/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2004  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#include "fuse_i.h"

#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/jiffies.h>
#include <linux/sched.h>

static struct inode_operations fuse_dir_inode_operations;
static struct inode_operations fuse_file_inode_operations;
static struct inode_operations fuse_symlink_inode_operations;

static struct file_operations fuse_dir_operations;

static struct dentry_operations fuse_dentry_operations;

/* FIXME: This should be user configurable */
#define FUSE_REVALIDATE_TIME (1 * HZ)

static void change_attributes(struct inode *inode, struct fuse_attr *attr)
{
	if(S_ISREG(inode->i_mode) && i_size_read(inode) != attr->size)
		invalidate_inode_pages2(inode->i_mapping);

	inode->i_mode    = (inode->i_mode & S_IFMT) + (attr->mode & 07777);
	inode->i_nlink   = attr->nlink;
	inode->i_uid     = attr->uid;
	inode->i_gid     = attr->gid;
	i_size_write(inode, attr->size);
	inode->i_blocks  = attr->blocks;
	inode->i_atime.tv_sec   = attr->atime;
	inode->i_atime.tv_nsec  = 0;
	inode->i_mtime.tv_sec   = attr->mtime;
	inode->i_mtime.tv_nsec  = 0;
	inode->i_ctime.tv_sec   = attr->ctime;
	inode->i_ctime.tv_nsec  = 0;
}

static void fuse_init_inode(struct inode *inode, struct fuse_attr *attr)
{
	inode->i_mode = attr->mode & S_IFMT;
	i_size_write(inode, attr->size);
	if(S_ISREG(inode->i_mode)) {
		inode->i_op = &fuse_file_inode_operations;
		fuse_init_file_inode(inode);
	}
	else if(S_ISDIR(inode->i_mode)) {
		inode->i_op = &fuse_dir_inode_operations;
		inode->i_fop = &fuse_dir_operations;
	}
	else if(S_ISLNK(inode->i_mode)) {
		inode->i_op = &fuse_symlink_inode_operations;
	}
	else {
		inode->i_op = &fuse_file_inode_operations;
		init_special_inode(inode, inode->i_mode,
				   new_decode_dev(attr->rdev));
	}
	inode->i_private = inode;
}

struct inode *fuse_iget(struct super_block *sb, ino_t ino,
			struct fuse_attr *attr, int version)
{
	struct inode *inode;

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);
	if ((inode->i_state & I_NEW)) {
		fuse_init_inode(inode, attr);
		inode->i_version = version;
		unlock_new_inode(inode);
	}

	change_attributes(inode, attr);

	return inode;
}

static int fuse_do_lookup(struct inode *dir, struct dentry *entry,
			  struct fuse_lookup_out *outarg, int *version)
{
	struct fuse_conn *fc = INO_FC(dir);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;

	if (entry->d_name.len > FUSE_NAME_MAX)
		return -ENAMETOOLONG;

	in.h.opcode = FUSE_LOOKUP;
	in.h.ino = dir->i_ino;
	in.numargs = 1;
	in.args[0].size = entry->d_name.len + 1;
	in.args[0].value = entry->d_name.name;
	out.numargs = 1;
	out.args[0].size = sizeof(struct fuse_lookup_out);
	out.args[0].value = outarg;
	request_send(fc, &in, &out);

	*version = out.h.unique;
	return out.h.error;
}

static int fuse_lookup_iget(struct inode *dir, struct dentry *entry,
			    struct inode **inodep)
{
	int err;
	struct fuse_lookup_out outarg;
	int version;
	struct inode *inode = NULL;

	err = fuse_do_lookup(dir, entry, &outarg, &version);
	if(!err) {
		inode = fuse_iget(dir->i_sb, outarg.ino, &outarg.attr, version);
		if (IS_ERR(inode))
			return PTR_ERR(inode);
	} else if(err != -ENOENT)
		return err;

	entry->d_time = jiffies;
	entry->d_op = &fuse_dentry_operations;
	*inodep = inode;
	return 0;
}

static void uncache_dir(struct inode *dir)
{
	struct dentry *entry = d_find_alias(dir);
	if (!entry)
		clear_nlink(dir);
	else {
		entry->d_time = jiffies - FUSE_REVALIDATE_TIME - 1;
		dput(entry);
	}
}

/* create needs to return a positive entry, so this is actually an
   mknod+lookup */
static int _fuse_mknod(struct inode *dir, struct dentry *entry, int mode,
		      dev_t rdev)
{
	struct fuse_conn *fc = INO_FC(dir);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_mknod_in inarg;
	struct fuse_mknod_out outarg;
	struct inode *inode;

	memset(&inarg, 0, sizeof(inarg));
	inarg.mode = mode;
	inarg.rdev = new_encode_dev(rdev);

	in.h.opcode = FUSE_MKNOD;
	in.h.ino = dir->i_ino;
	in.numargs = 2;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	in.args[1].size = entry->d_name.len + 1;
	in.args[1].value = entry->d_name.name;
	out.numargs = 1;
	out.args[0].size = sizeof(outarg);
	out.args[0].value = &outarg;
	request_send(fc, &in, &out);

	if(out.h.error)
		return out.h.error;

	inode = fuse_iget(dir->i_sb, outarg.ino, &outarg.attr, out.h.unique);
	if (IS_ERR(inode))
		return PTR_ERR(inode);

	/* Don't allow userspace to do really stupid things... */
	if((inode->i_mode ^ mode) & S_IFMT) {
		iput(inode);
		printk("fuse_mknod: inode has wrong type\n");
		return -EPROTO;
	}

	d_instantiate(entry, inode);
	uncache_dir(dir);
	return 0;
}

static int _fuse_create(struct inode *dir, struct dentry *entry, int mode)
{
	return _fuse_mknod(dir, entry, mode, 0);
}

/* knfsd needs the new entry instantiated in mkdir/symlink/link. this
   should rather be done like mknod: attributes returned in out arg to
   save a call to userspace */
static int lookup_new_entry(struct inode *dir, struct dentry *entry)
{
	struct inode *inode;
	int err = fuse_lookup_iget(dir, entry, &inode);
	if(err || !inode) {
		printk("fuse_mkdir: failed to look up new entry\n");
		return err ? err : -ENOENT;
	}
	d_instantiate(entry, inode);
	uncache_dir(dir);
	return 0;
}

static int fuse_mkdir(struct inode *dir, struct dentry *entry, int mode)
{
	struct fuse_conn *fc = INO_FC(dir);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_mkdir_in inarg;

	memset(&inarg, 0, sizeof(inarg));
	inarg.mode = mode;

	in.h.opcode = FUSE_MKDIR;
	in.h.ino = dir->i_ino;
	in.numargs = 2;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	in.args[1].size = entry->d_name.len + 1;
	in.args[1].value = entry->d_name.name;
	request_send(fc, &in, &out);
	if(out.h.error)
		return out.h.error;

	return lookup_new_entry(dir, entry);
}

static int fuse_symlink(struct inode *dir, struct dentry *entry,
			const char *link)
{
	struct fuse_conn *fc = INO_FC(dir);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	unsigned int len = strlen(link) + 1;

	if (len > FUSE_SYMLINK_MAX)
		return -ENAMETOOLONG;

	in.h.opcode = FUSE_SYMLINK;
	in.h.ino = dir->i_ino;
	in.numargs = 2;
	in.args[0].size = entry->d_name.len + 1;
	in.args[0].value = entry->d_name.name;
	in.args[1].size = len;
	in.args[1].value = link;
	request_send(fc, &in, &out);
	if(out.h.error)
		return out.h.error;

	return lookup_new_entry(dir, entry);
}

static int fuse_remove(struct inode *dir, struct dentry *entry,
		       enum fuse_opcode op)
{
	struct fuse_conn *fc = INO_FC(dir);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;

	in.h.opcode = op;
	in.h.ino = dir->i_ino;
	in.numargs = 1;
	in.args[0].size = entry->d_name.len + 1;
	in.args[0].value = entry->d_name.name;
	request_send(fc, &in, &out);

	return out.h.error;
}

static int fuse_unlink(struct inode *dir, struct dentry *entry)
{
	int err = fuse_remove(dir, entry, FUSE_UNLINK);
	if(!err) {
		/* Set nlink to zero so the inode can be cleared, if
                   the inode does have more links this will be
                   discovered at the next lookup/getattr */
		clear_nlink(entry->d_inode);

		uncache_dir(dir);
		return 0;
	}
	return err;
}

static int fuse_rmdir(struct inode *dir, struct dentry *entry)
{
	int err = fuse_remove(dir, entry, FUSE_RMDIR);
	if(!err) {
		clear_nlink(entry->d_inode);
		uncache_dir(dir);
	}
	return err;
}

static int fuse_rename(struct inode *olddir, struct dentry *oldent,
		       struct inode *newdir, struct dentry *newent)
{
	struct fuse_conn *fc = INO_FC(olddir);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_rename_in inarg;

	memset(&inarg, 0, sizeof(inarg));
	inarg.newdir = newdir->i_ino;

	in.h.opcode = FUSE_RENAME;
	in.h.ino = olddir->i_ino;
	in.numargs = 3;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	in.args[1].size = oldent->d_name.len + 1;
	in.args[1].value = oldent->d_name.name;
	in.args[2].size = newent->d_name.len + 1;
	in.args[2].value = newent->d_name.name;
	request_send(fc, &in, &out);

	if (!out.h.error) {
		uncache_dir(olddir);
		if (olddir != newdir)
			uncache_dir(newdir);
	}

	return out.h.error;
}

static int fuse_link(struct dentry *entry, struct inode *newdir,
		     struct dentry *newent)
{
	struct inode *inode = entry->d_inode;
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_link_in inarg;

	memset(&inarg, 0, sizeof(inarg));
	inarg.newdir = newdir->i_ino;

	in.h.opcode = FUSE_LINK;
	in.h.ino = inode->i_ino;
	in.numargs = 2;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	in.args[1].size = newent->d_name.len + 1;
	in.args[1].value = newent->d_name.name;
	request_send(fc, &in, &out);
	if(out.h.error)
		return out.h.error;

	/* Invalidate old entry, so attributes are refreshed */
	d_invalidate(entry);
	return lookup_new_entry(newdir, newent);
}

int fuse_do_getattr(struct inode *inode)
{
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_getattr_out arg;

	in.h.opcode = FUSE_GETATTR;
	in.h.ino = inode->i_ino;
	out.numargs = 1;
	out.args[0].size = sizeof(arg);
	out.args[0].value = &arg;
	request_send(fc, &in, &out);

	if(!out.h.error)
		change_attributes(inode, &arg.attr);

	return out.h.error;
}

static int fuse_revalidate(struct dentry *entry)
{
	struct inode *inode = entry->d_inode;
	struct fuse_conn *fc = INO_FC(inode);

	if(inode->i_ino == FUSE_ROOT_INO) {
		if(!(fc->flags & FUSE_ALLOW_OTHER) &&
		   current_fsuid() != fc->uid)
			return -EACCES;
	} else if(!(fc->flags & COFS_MOUNT_NOCACHE) &&
		  time_before_eq(jiffies, entry->d_time + FUSE_REVALIDATE_TIME))
		return 0;

	return fuse_do_getattr(inode);
}

static int fuse_permission(struct inode *inode, int mask)
{
	struct fuse_conn *fc = INO_FC(inode);

	if(!(fc->flags & FUSE_ALLOW_OTHER) && current_fsuid() != fc->uid)
		return -EACCES;
	else if(fc->flags & FUSE_DEFAULT_PERMISSIONS) {
		int err = generic_permission(inode, mask, NULL);

		/* If permission is denied, try to refresh file
		   attributes.  This is also needed, because the root
		   node will at first have no permissions */

		if(err == -EACCES) {
		 	err = fuse_do_getattr(inode);
			if(!err)
			 	err = generic_permission(inode, mask, NULL);
		}

		/* FIXME: Need some mechanism to revoke permissions:
		   currently if the filesystem suddenly changes the
		   file mode, we will not be informed abot that, and
		   continue to allow access to the file/directory.

		   This is actually not so grave, since the user can
		   simply keep access to the file/directory anyway by
		   keeping it open... */

		return err;
	}
	else
		return 0;
}

static int parse_dirfile(char *buf, size_t nbytes, struct file *file,
			 void *dstbuf, filldir_t filldir)
{
	while(nbytes >= FUSE_NAME_OFFSET) {
		struct fuse_dirent *dirent = (struct fuse_dirent *) buf;
		size_t reclen = FUSE_DIRENT_SIZE(dirent);
		int over;

		if(dirent->namelen > NAME_MAX) {
			printk("parse_dirfile: name too long\n");
			return -EPROTO;
		}
		if(reclen > nbytes)
			break;

		over = filldir(dstbuf, dirent->name, dirent->namelen,
			      file->f_pos, dirent->ino, dirent->type);
		if(over)
			break;

		buf += reclen;
		file->f_pos += reclen;
		nbytes -= reclen;
	}

	return 0;
}

#define DIR_BUFSIZE 4096

typedef struct {
	struct fuse_conn *fc;
	int inode;
} readdir_data_t;

static int fuse_readdir(struct file *file, void *dstbuf, filldir_t filldir)
{
	readdir_data_t *rd = file->private_data;
	unsigned long flags;
	int ret, size;
	char *buf;

	buf = kmalloc(DIR_BUFSIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	co_passage_page_assert_valid();

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_DEVICE;
	co_passage_page->params[0] = CO_DEVICE_FILESYSTEM;
	co_passage_page->params[1] = rd->fc->cofs_unit;
	co_passage_page->params[2] = FUSE_DIR_READ;
	co_passage_page->params[3] = rd->inode;
	co_passage_page->params[5] = DIR_BUFSIZE;
	co_passage_page->params[6] = (unsigned long)buf;
	co_passage_page->params[8] = file->f_pos;

	co_switch_wrapper();

	ret = co_passage_page->params[4];
	size = co_passage_page->params[7];

	co_passage_page_release(flags);

	if (ret) {
		printk("fuse_readdir: host returned error: %x\n", ret);
		kfree(buf);
		return ret;
	}

	parse_dirfile(buf, size, file, dstbuf, filldir);

	ret = 0;
	kfree(buf);
	return ret;
}

static char *read_link(struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	char *link;

	link = (char *) __get_free_page(GFP_KERNEL);
	if(!link)
		return ERR_PTR(-ENOMEM);

	in.h.opcode = FUSE_READLINK;
	in.h.ino = inode->i_ino;
	out.argvar = 1;
	out.numargs = 1;
	out.args[0].size = PAGE_SIZE - 1;
	out.args[0].value = link;
	request_send(fc, &in, &out);
	if(out.h.error) {
		free_page((unsigned long) link);
		return ERR_PTR(out.h.error);
	}

	link[out.args[0].size] = '\0';
	return link;
}

static void free_link(char *link)
{
	if(!IS_ERR(link))
		free_page((unsigned long) link);
}

static int fuse_readlink(struct dentry *dentry, char *buffer, int buflen)
{
	int ret;
	char *link;

	link = read_link(dentry);
	ret = vfs_readlink(dentry, buffer, buflen, link);
	free_link(link);
	return ret;
}

static void * fuse_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	int ret;
	char *link;

	link = read_link(dentry);
	ret = vfs_follow_link(nd, link);
	free_link(link);
	return ERR_PTR(ret);
}

static int fuse_dir_open(struct inode *inode, struct file *file)
{
	struct fuse_conn *fc = INO_FC(inode);
	unsigned long flags;
	readdir_data_t *rd;
	int ret;

	rd = kmalloc(sizeof(*rd), GFP_KERNEL);
	if (!rd)
		return -ENOMEM;

	rd->fc = fc;
	rd->inode = inode->i_ino;

	co_passage_page_assert_valid();

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_DEVICE;
	co_passage_page->params[0] = CO_DEVICE_FILESYSTEM;
	co_passage_page->params[1] = fc->cofs_unit;
	co_passage_page->params[2] = FUSE_DIR_OPEN;
	co_passage_page->params[3] = inode->i_ino;
	co_passage_page->params[4] = 0;

	co_switch_wrapper();

	ret = co_passage_page->params[4];

	co_passage_page_release(flags);

	if (ret) {
		printk("fuse_dir_open: host returned error: %x\n", ret);
		kfree(rd);
	} else {
		file->private_data = (void *)rd;
	}

	return ret;
}

static int fuse_dir_release(struct inode *inode, struct file *file)
{
	readdir_data_t *rd = file->private_data;
	unsigned long flags;
	int ret;

	co_passage_page_assert_valid();

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_DEVICE;
	co_passage_page->params[0] = CO_DEVICE_FILESYSTEM;
	co_passage_page->params[1] = rd->fc->cofs_unit;
	co_passage_page->params[2] = FUSE_DIR_RELEASE;
	co_passage_page->params[3] = rd->inode;
	co_passage_page->params[4] = 0;

	co_switch_wrapper();

	ret = co_passage_page->params[4];

	co_passage_page_release(flags);

	if (ret)
		printk("fuse_dir_release: host returned error: %x\n", ret);

	kfree(rd);

	return ret;
}

static unsigned int iattr_to_fattr(struct iattr *iattr,
				   struct fuse_attr *fattr)
{
	unsigned int ivalid = iattr->ia_valid;
	unsigned int fvalid = 0;

	memset(fattr, 0, sizeof(*fattr));

	if(ivalid & ATTR_MODE)
		fvalid |= FATTR_MODE,   fattr->mode = iattr->ia_mode;
	if(ivalid & ATTR_UID)
		fvalid |= FATTR_UID,    fattr->uid = iattr->ia_uid;
	if(ivalid & ATTR_GID)
		fvalid |= FATTR_GID,    fattr->gid = iattr->ia_gid;
	if(ivalid & ATTR_SIZE)
		fvalid |= FATTR_SIZE,   fattr->size = iattr->ia_size;
	/* You can only _set_ these together (they may change by themselves) */
	if((ivalid & (ATTR_ATIME | ATTR_MTIME)) == (ATTR_ATIME | ATTR_MTIME)) {
		fvalid |= FATTR_UTIME;
		fattr->atime = iattr->ia_atime.tv_sec;
		fattr->mtime = iattr->ia_mtime.tv_sec;
	}

	return fvalid;
}

static int fuse_setattr(struct dentry *entry, struct iattr *attr)
{
	struct inode *inode = entry->d_inode;
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_setattr_in inarg;
	struct fuse_setattr_out outarg;

	/* FIXME: need to fix race between truncate and writepage */
	if (attr->ia_valid & ATTR_SIZE)
		fuse_sync_inode(inode);

	memset(&inarg, 0, sizeof(inarg));
	inarg.valid = iattr_to_fattr(attr, &inarg.attr);

	in.h.opcode = FUSE_SETATTR;
	in.h.ino = inode->i_ino;
	in.numargs = 1;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	out.numargs = 1;
	out.args[0].size = sizeof(outarg);
	out.args[0].value = &outarg;
	request_send(fc, &in, &out);

	if(!out.h.error) {
		if(attr->ia_valid & ATTR_SIZE &&
		   outarg.attr.size < i_size_read(inode))
			vmtruncate(inode, outarg.attr.size);

		change_attributes(inode, &outarg.attr);
		d_invalidate(entry);
	}

	return out.h.error;
}

static int _fuse_dentry_revalidate(struct dentry *entry)
{
	struct fuse_conn *fc;
	struct inode *inode = entry->d_inode;

	if(!inode)
		return 0;

	fc = INO_FC(inode);
	if((fc->flags & COFS_MOUNT_NOCACHE) ||
	   time_after(jiffies, entry->d_time + FUSE_REVALIDATE_TIME)) {
		struct fuse_lookup_out outarg;
		int version;
		int ret;

		ret = fuse_do_lookup(entry->d_parent->d_inode, entry, &outarg,
				     &version);
		if(ret)
			return 0;

		if(outarg.ino != inode->i_ino)
			return 0;

		change_attributes(inode, &outarg.attr);
		inode->i_version = version;
		entry->d_time = jiffies;
	}
	return 1;
}

#define fuse_mknod _fuse_mknod

static int fuse_getattr(struct vfsmount *mnt, struct dentry *entry,
			struct kstat *stat)
{
	struct inode *inode = entry->d_inode;
	int err = fuse_revalidate(entry);
	if(!err)
		generic_fillattr(inode, stat);

	return err;
}

static struct dentry *fuse_lookup(struct inode *dir, struct dentry *entry,
				   struct nameidata *nd)
{
	struct inode *inode;
	int err = fuse_lookup_iget(dir, entry, &inode);
	if (err)
		return ERR_PTR(err);
	return d_splice_alias(inode, entry);
}

static int fuse_create(struct inode *dir, struct dentry *entry, int mode,
		       struct nameidata *nd)
{
	return _fuse_create(dir, entry, mode);
}

static int fuse_dentry_revalidate(struct dentry *entry, struct nameidata *nd)
{
	return _fuse_dentry_revalidate(entry);
}


static struct inode_operations fuse_dir_inode_operations =
{
	.lookup		= fuse_lookup,
	.create		= fuse_create,
	.mknod		= fuse_mknod,
	.mkdir		= fuse_mkdir,
	.symlink	= fuse_symlink,
	.unlink		= fuse_unlink,
	.rmdir		= fuse_rmdir,
	.rename		= fuse_rename,
	.link		= fuse_link,
	.setattr	= fuse_setattr,
	.permission	= fuse_permission,
	.getattr	= fuse_getattr,
};

static struct file_operations fuse_dir_operations = {
	.read		= generic_read_dir,
	.readdir	= fuse_readdir,
	.open		= fuse_dir_open,
	.release	= fuse_dir_release,
};

static struct inode_operations fuse_file_inode_operations = {
	.setattr	= fuse_setattr,
	.permission	= fuse_permission,
	.getattr	= fuse_getattr,
};

static struct inode_operations fuse_symlink_inode_operations =
{
	.setattr	= fuse_setattr,
	.readlink	= fuse_readlink,
	.follow_link	= fuse_follow_link,
	.getattr	= fuse_getattr,
};

static struct dentry_operations fuse_dentry_operations = {
	.d_revalidate	= fuse_dentry_revalidate,
};

/*
 * Local Variables:
 * indent-tabs-mode: t
 * c-basic-offset: 8
 * End:
 */
