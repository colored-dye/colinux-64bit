/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001	Miklos Szeredi (miklos@szeredi.hu)

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#include "fuse_i.h"

#include <linux/pagemap.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/statfs.h>
#include <linux/exportfs.h>

#define FUSE_SUPER_MAGIC 0x65735546

#ifndef FS_BINARY_MOUNTDATA
#define FS_BINARY_MOUNTDATA 0
#endif

static void fuse_clear_inode(struct inode *inode)
{
	unsigned long flags;
	struct fuse_conn *fc = INO_FC(inode);

	if (FUSE_ROOT_INO == inode->i_ino)
		return;

	co_passage_page_assert_valid();
	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_DEVICE;
	co_passage_page->params[0] = CO_DEVICE_FILESYSTEM;
	co_passage_page->params[1] = fc->cofs_unit;
	co_passage_page->params[2] = FUSE_FORGET;
	co_passage_page->params[3] = inode->i_ino;
	co_switch_wrapper();
	co_passage_page_release(flags);
}

static void fuse_put_super(struct super_block *sb)
{
	struct fuse_conn *fc = SB_FC(sb);

	spin_lock(&fuse_lock);
	fc->sb = NULL;
	fc->uid = 0;
	fc->flags = 0;
	/* Flush all readers on this fs */
	release_conn(fc);
	SB_FC(sb) = NULL;
	spin_unlock(&fuse_lock);
}

static void convert_fuse_statfs(struct kstatfs *stbuf, struct fuse_kstatfs *attr)
{
	stbuf->f_type	 = FUSE_SUPER_MAGIC;
	stbuf->f_bsize	 = attr->block_size;
	stbuf->f_blocks	 = attr->blocks;
	stbuf->f_bfree	 = stbuf->f_bavail = attr->blocks_free;
	stbuf->f_files	 = attr->files;
	stbuf->f_ffree	 = attr->files_free;
	/* Is this field necessary?  Most filesystems ignore it...
	stbuf->f_fsid.val[0] = (FUSE_SUPER_MAGIC>>16)&0xffff;
	stbuf->f_fsid.val[1] =	FUSE_SUPER_MAGIC     &0xffff; */
	stbuf->f_namelen = attr->namelen;
}

static int fuse_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	struct fuse_conn *fc = SB_FC(sb);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_statfs_out outarg;

	in.numargs = 0;
	in.h.opcode = FUSE_STATFS;
	out.numargs = 1;
	out.args[0].size = sizeof(outarg);
	out.args[0].value = &outarg;
	request_send(fc, &in, &out);
	if(!out.h.error)
		convert_fuse_statfs(buf, &outarg.st);

	return out.h.error;
}

static int _atoi(const char *s, const char **out)
{
	/* lib/spprintf.h */

        int i=0;

        while (isdigit(*s))
                i = i*10 + *(s++) - '0';

	*out = s;

        return i;
}

static struct fuse_conn *co_get_conn(struct cofuse_mount_data *d)
{
	int index;
	int ret;
	unsigned long flags;
	struct fuse_conn *conn = NULL;
	const char *name, *next;

	if (d == NULL) {
		printk("co_get_conn: Bad mount data\n");
		return NULL;
	}

	name = d->name;

	if (strncmp("cofs", name, 4) == 0)
		name += 4;

	index = _atoi(name, &next);
	if (index < 0  || index >= CO_MODULE_MAX_COFS) {
		printk("co_get_conn: Invalid index %d\n", index);
		return NULL;
	}

	if (cofs_volumes[index])
		return cofs_volumes[index];

	conn = kzalloc(sizeof(struct fuse_conn), GFP_KERNEL);
	if (!conn)
		return NULL;

	if (*next == ':')
		snprintf(conn->opt_pathname, sizeof(conn->opt_pathname), "%s", next+1);

	conn->cofs_unit = index;
	conn->flags = d->flags;

	if (d->flags & COFS_MOUNT_NOCACHE)
		printk("cofs%d: cache disabed\n", index);

	co_passage_page_assert_valid();
	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_DEVICE;
	co_passage_page->params[0] = CO_DEVICE_FILESYSTEM;
	co_passage_page->params[1] = conn->cofs_unit;
	co_passage_page->params[2] = FUSE_MOUNT;
	co_passage_page->params[5] = d->uid;
	co_passage_page->params[6] = d->gid;
	co_passage_page->params[7] = d->dir_mode;
	co_passage_page->params[8] = d->file_mode;
	co_passage_page->params[9] = d->flags;
	memcpy(&co_passage_page->params[30], conn->opt_pathname, strlen(conn->opt_pathname) + 1);
	co_switch_wrapper();
	ret = co_passage_page->params[4];
	co_passage_page_release(flags);

	if (ret) {
		kfree(conn);
		conn = NULL;
	}
	cofs_volumes[index] = conn;

	return conn;
}

static struct inode *get_root_inode(struct super_block *sb, unsigned int mode)
{
	struct fuse_attr attr;
	memset(&attr, 0, sizeof(attr));

	attr.mode = mode;
	return fuse_iget(sb, 1, &attr, 0);
}

static struct super_operations fuse_super_operations = {
	.clear_inode	= fuse_clear_inode,
	.put_super	= fuse_put_super,
	.statfs		= fuse_statfs,
};

static int fuse_read_super(struct super_block *sb, void *data, int silent)
{
	struct fuse_conn *fc;
	struct inode *root;
	struct cofuse_mount_data *co_d = data;
	struct fuse_mount_data *d = co_d->fuse;
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = FUSE_SUPER_MAGIC;
	sb->s_op = &fuse_super_operations;
	sb->s_maxbytes = MAX_LFS_FILESIZE;

	fc = co_get_conn(co_d);
	if(fc == NULL)
		return -EINVAL;
	spin_lock(&fuse_lock);
	if(fc->sb != NULL) {
		printk("fuse_read_super: connection already mounted\n");
		spin_unlock(&fuse_lock);
		return -EINVAL;
	}
	fc->sb = sb;
	fc->flags = d->flags;
	fc->uid = d->uid;
	spin_unlock(&fuse_lock);

	/* fc is needed in fuse_init_file_inode which could be called
	   from get_root_inode */
	SB_FC(sb) = fc;

	root = get_root_inode(sb, d->rootmode);
	if(root == NULL) {
		printk("fuse_read_super: failed to get root inode\n");
		return -EINVAL;
	}

	sb->s_root = d_alloc_root(root);
	if(!sb->s_root) {
		printk("fuse_read_super: failed to allocate root\n");
		return -EINVAL;
	}

	return 0;
}

/*
 * cofuse_getopt and cofuse_parse_options were
 * addopted from smb
 */

struct option {
	const char *name;
	unsigned long flag;
	int val;
};

/**
 *      cofuse_getopt - option parser
 *      based on smb_getopt from fs/smbfs
 *
 *      @caller: name of the caller, for error messages
 *      @options: the options string
 *      @opts: an array of &struct option entries controlling parser operations
 *      @optopt: output; will contain the current option
 *      @optarg: output; will contain the value (if one exists)
 *      @flag: output; may be NULL; should point to a long for or'ing flags
 *      @value: output; may be NULL; will be overwritten with the integer value
 *              of the current argument.
 *
 *      Helper to parse options on the format used by mount ("a=b,c=d,e,f").
 *      Returns opts->val if a matching entry in the 'opts' array is found,
 *      0 when no more tokens are found, -1 if an error is encountered.
 */
static int cofuse_getopt(char *caller, char **options, struct option *opts,
			 char **optopt, char **optarg, unsigned long *flag,
			 unsigned long *value)
{
        char *token;
        char *val;
        int i;

        do {
                if ((token = strsep(options, ",")) == NULL)
                        return 0;
        } while (*token == '\0');
        *optopt = token;

        *optarg = NULL;
        if ((val = strchr (token, '=')) != NULL) {
                *val++ = 0;
                if (value)
                        *value = simple_strtoul(val, NULL, 0);
                *optarg = val;
        }

        for (i = 0; opts[i].name != NULL; i++) {
                if (!strcmp(opts[i].name, token)) {
                        if (!opts[i].flag && (!val || !*val)) {
                                printk("%s: the %s option requires an argument\n",
                                       caller, token);
                                return -1;
                        }

                        if (flag && opts[i].flag)
                                *flag |= opts[i].flag;

                        return opts[i].val;
                }
        }
        printk("%s: Unrecognized mount option %s\n", caller, token);
        return -1;
}

static struct option opts[] = {
	{ "uid",	0, 'u' },
	{ "gid",	0, 'g' },
	{ "fmask",	0, 'f' },
	{ "dmask",	0, 'd' },
	{ "nocache",	COFS_MOUNT_NOCACHE, 1 },
	{ "noattrib",	COFS_MOUNT_NOATTRIB, 1 },
	{ NULL,		0, 0}
};

/*
 * parse_options - based on parse_options from fs/smbfs
 */
static int parse_options(struct cofuse_mount_data *mnt, char *options)
{
        int c;
        unsigned long flags = 0;
        unsigned long value = 0;
        char *optarg;
        char *optopt;

        while ((c = cofuse_getopt("cofuse", &options, opts,
				  &optopt, &optarg, &flags, &value)) > 0)
	{
                switch (c) {
                case 1:
                        /* got a "flag" option */
                        break;
                case 'u':
                        mnt->uid = value;
                        break;
                case 'g':
                        mnt->gid = value;
                        break;
                case 'f':
                        mnt->file_mode = (value & S_IRWXUGO) | S_IFREG;
                        break;
                case 'd':
                        mnt->dir_mode = (value & S_IRWXUGO) | S_IFDIR;
                        break;
                default:
                        printk("cofs: Unrecognized mount option %s\n", optopt);
                        return -1;
                }
        }

        mnt->flags = flags;
        return c;
}

static int fuse_get_sb(struct file_system_type *fs_type,
				       int flags, const char *dev_name,
				       void *raw_data, struct vfsmount *mnt)
{
	struct cofuse_mount_data co_md = {0, };
	struct fuse_mount_data md = {0, };
	int ret;

	co_md.uid = current_uid();
	co_md.gid = current_gid();
	co_md.dir_mode = FUSE_S_IRWXU | FUSE_S_IRGRP | FUSE_S_IXGRP |
		FUSE_S_IROTH | FUSE_S_IXOTH | S_IFDIR;
	co_md.file_mode = FUSE_S_IRWXU | FUSE_S_IRGRP | FUSE_S_IXGRP |
		FUSE_S_IROTH | FUSE_S_IXOTH | S_IFREG;

	ret = parse_options(&co_md, raw_data);
	if (ret == -1)
		return -EINVAL;

	md.rootmode = S_IFDIR | S_IRWXU | S_IRUGO | S_IXUGO;
	md.flags = FUSE_ALLOW_OTHER | FUSE_DEFAULT_PERMISSIONS | co_md.flags;

	co_md.fuse = &md;
	snprintf(co_md.name, sizeof(co_md.name), "%s", dev_name);

	return get_sb_nodev(fs_type, flags, &co_md, fuse_read_super, mnt);
}

static struct file_system_type fuse_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "cofs",
	.get_sb		= fuse_get_sb,
	.kill_sb	= kill_anon_super,
	.fs_flags	= FS_BINARY_MOUNTDATA,
};

int __init fuse_fs_init()
{
	int res;

	res = register_filesystem(&fuse_fs_type);
	if(res)
		printk("fuse: failed to register filesystem\n");

	return res;
}

void fuse_fs_cleanup()
{
	unregister_filesystem(&fuse_fs_type);
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * c-basic-offset: 8
 * End:
 */
