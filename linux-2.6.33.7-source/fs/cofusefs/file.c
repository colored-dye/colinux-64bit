/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2004  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/
#include "fuse_i.h"

#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>

static int fuse_open(struct inode *inode, struct file *file)
{
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_open_in inarg;
	int err;

	err = generic_file_open(inode, file);
	if(err)
		return err;

	/* If opening the root node, no lookup has been performed on
	   it, so the attributes must be refreshed */
	if(inode->i_ino == FUSE_ROOT_INO) {
		int err = fuse_do_getattr(inode);
		if(err)
		 	return err;
	}

	memset(&inarg, 0, sizeof(inarg));
	inarg.flags = file->f_flags & ~O_EXCL;

	in.h.opcode = FUSE_OPEN;
	in.h.ino = inode->i_ino;
	in.numargs = 1;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	request_send(fc, &in, &out);
	if(!out.h.error && !(fc->flags & FUSE_KERNEL_CACHE)) {
		invalidate_inode_pages2(inode->i_mapping);
	}

	return out.h.error;
}

void fuse_sync_inode(struct inode *inode)
{
	filemap_fdatawrite(inode->i_mapping);
	filemap_fdatawait(inode->i_mapping);
}

static int fuse_release(struct inode *inode, struct file *file)
{
	if(file->f_mode & FMODE_WRITE)
		fuse_sync_inode(inode);

	return 0;
}

static int fuse_fsync(struct file *file, struct dentry *de, int datasync)
{

	/* FIXME: need to ensure, that all write requests issued
           before this request are completed.  Should userspace take
           care of this? */

	return 0;
}

static int fuse_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_read_in inarg;
	char *buffer;

	buffer = kmap(page);

	memset(&inarg, 0, sizeof(inarg));
	inarg.offset = (unsigned long long) page->index << PAGE_CACHE_SHIFT;
	inarg.size = PAGE_CACHE_SIZE;

	in.h.opcode = FUSE_READ;
	in.h.ino = inode->i_ino;
	in.numargs = 1;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	out.argvar = 1;
	out.numargs = 1;
	out.args[0].size = PAGE_CACHE_SIZE;
	out.args[0].value = buffer;

	/* Hack: Can't detect readed bytes. But the overhead should fill with zero. */
	if (inarg.offset + PAGE_CACHE_SIZE > i_size_read(inode))
		memset(buffer, 0, PAGE_CACHE_SIZE);

	request_send(fc, &in, &out);
	if(!out.h.error) {
		flush_dcache_page(page);
		SetPageUptodate(page);
	}

	kunmap(page);
	unlock_page(page);

	return out.h.error;
}

static int fuse_is_block_uptodate(struct address_space *mapping,
		struct inode *inode, size_t bl_index)
{
	size_t index = bl_index << FUSE_BLOCK_PAGE_SHIFT;
	size_t end_index = ((bl_index + 1) << FUSE_BLOCK_PAGE_SHIFT) - 1;
	size_t file_end_index = i_size_read(inode) >> PAGE_CACHE_SHIFT;

	if (end_index > file_end_index)
		end_index = file_end_index;

	for (; index <= end_index; index++) {
		struct page *page = find_get_page(mapping, index);

		if (!page)
			return 0;

		if (!PageUptodate(page)) {
			page_cache_release(page);
			return 0;
		}

		page_cache_release(page);
	}

	return 1;
}


static int fuse_cache_block(struct address_space *mapping,
		struct inode *inode, char *bl_buf,
		size_t bl_index)
{
	size_t start_index = bl_index << FUSE_BLOCK_PAGE_SHIFT;
	size_t end_index = ((bl_index + 1) << FUSE_BLOCK_PAGE_SHIFT) - 1;
	size_t file_end_index = i_size_read(inode) >> PAGE_CACHE_SHIFT;

	int i;

	if (end_index > file_end_index)
		end_index = file_end_index;

	for (i = 0; start_index + i <= end_index; i++) {
		size_t index = start_index + i;
		struct page *page;
		char *buffer;

		page = grab_cache_page(mapping, index);
		if (!page)
			return -1;

		if (!PageUptodate(page)) {
			buffer = kmap(page);
			memcpy(buffer, bl_buf + i * PAGE_CACHE_SIZE,
					PAGE_CACHE_SIZE);
			flush_dcache_page(page);
			SetPageUptodate(page);
			kunmap(page);
		}

		unlock_page(page);
		page_cache_release(page);
	}

	return 0;
}

static int fuse_file_read_block(struct inode *inode, char *bl_buf,
		size_t bl_index)
{
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_read_in inarg;

	memset(&inarg, 0, sizeof(inarg));
	inarg.offset = (unsigned long long) bl_index << FUSE_BLOCK_SHIFT;
	inarg.size = FUSE_BLOCK_SIZE;

	in.h.opcode = FUSE_READ;
	in.h.ino = inode->i_ino;
	in.numargs = 1;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	out.argvar = 1;
	out.numargs = 1;
	out.args[0].size = FUSE_BLOCK_SIZE;
	out.args[0].value = bl_buf;

	/* Hack: Can't detect readed bytes. But the overhead should fill with zero. */
	if (inarg.offset + FUSE_BLOCK_SIZE > i_size_read(inode))
		memset(bl_buf, 0, FUSE_BLOCK_SIZE);

	request_send(fc, &in, &out);

	return out.h.error;
}

static void fuse_file_bigread(struct address_space *mapping,
			      struct inode *inode, loff_t pos, size_t count)
{
	size_t bl_index = pos >> FUSE_BLOCK_SHIFT;
	size_t bl_end_index = (pos + count) >> FUSE_BLOCK_SHIFT;
	size_t bl_file_end_index = i_size_read(inode) >> FUSE_BLOCK_SHIFT;

	if (bl_end_index > bl_file_end_index)
		bl_end_index = bl_file_end_index;

	while (bl_index <= bl_end_index) {
		int res;
		char *bl_buf = kmalloc(FUSE_BLOCK_SIZE, GFP_NOFS);
		if (!bl_buf)
			break;
		res = fuse_is_block_uptodate(mapping, inode, bl_index);
		if (!res)
			res = fuse_file_read_block(inode, bl_buf, bl_index);
		if (!res)
			fuse_cache_block(mapping, inode, bl_buf, bl_index);
		kfree(bl_buf);
		bl_index++;
	}
}

static ssize_t fuse_file_read(struct file *filp, char *buf,
		size_t count, loff_t * ppos)
{
	struct address_space *mapping = filp->f_dentry->d_inode->i_mapping;
	struct inode *inode = mapping->host;
	struct fuse_conn *fc = INO_FC(inode);

	if(fc->flags & FUSE_LARGE_READ) {
		/* Don't allow this to get mixed up with writes */
		mutex_lock(&inode->i_mutex);
		fuse_file_bigread(mapping, inode, *ppos, count);
		mutex_unlock(&inode->i_mutex);
	}

	return do_sync_read(filp, buf, count, ppos);
}

static int write_buffer(struct inode *inode, struct page *page,
			unsigned offset, size_t count)
{
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in in = FUSE_IN_INIT;
	struct fuse_out out = FUSE_OUT_INIT;
	struct fuse_write_in inarg;
	char *buffer;

	buffer = kmap(page);

	memset(&inarg, 0, sizeof(inarg));
	inarg.offset = ((unsigned long long) page->index << PAGE_CACHE_SHIFT) +
		offset;
	inarg.size = count;

	in.h.opcode = FUSE_WRITE;
	in.h.ino = inode->i_ino;
	in.numargs = 2;
	in.args[0].size = sizeof(inarg);
	in.args[0].value = &inarg;
	in.args[1].size = count;
	in.args[1].value = buffer + offset;
	request_send(fc, &in, &out);
	kunmap(page);
	if(out.h.error)
		SetPageError(page);

	return out.h.error;
}

static int get_write_count(struct inode *inode, struct page *page)
{
	unsigned long end_index;
	loff_t size = i_size_read(inode);
	int count;

	end_index = size >> PAGE_CACHE_SHIFT;
	if(page->index < end_index)
		count = PAGE_CACHE_SIZE;
	else {
		count = size & (PAGE_CACHE_SIZE - 1);
		if(page->index > end_index || count == 0)
			return 0;
	}
	return count;
}

static void write_buffer_end(struct fuse_conn *fc, struct fuse_in *in,
			     struct fuse_out *out, void *_page)
{
	struct page *page = (struct page *) _page;

	if(out->h.error) {
		SetPageError(page);
		if(out->h.error == -ENOSPC)
			set_bit(AS_ENOSPC, &page->mapping->flags);
		else
			set_bit(AS_EIO, &page->mapping->flags);
	}
	end_page_writeback(page);
	kunmap(page);
	kfree(in);
}

static int write_buffer_nonblock(struct inode *inode, struct page *page,
				 unsigned offset, size_t count)
{
	int err;
	struct fuse_conn *fc = INO_FC(inode);
	struct fuse_in *in = NULL;
	struct fuse_out *out = NULL;
	struct fuse_write_in *inarg = NULL;
	char *buffer;
	unsigned int s = sizeof(struct fuse_in) + sizeof(struct fuse_out) +
		sizeof(struct fuse_write_in);

	in = kzalloc(s, GFP_NOFS);
	if(!in)
		return -ENOMEM;

	out = (struct fuse_out *)(in + 1);
	inarg = (struct fuse_write_in *)(out + 1);

	buffer = kmap(page);

	inarg->offset = ((unsigned long long) page->index << PAGE_CACHE_SHIFT) + offset;
	inarg->size = count;

	in->h.opcode = FUSE_WRITE;
	in->h.ino = inode->i_ino;
	in->numargs = 2;
	in->args[0].size = sizeof(struct fuse_write_in);
	in->args[0].value = inarg;
	in->args[1].size = count;
	in->args[1].value = buffer + offset;
	err = request_send_nonblock(fc, in, out, write_buffer_end, page);
	if(err) {
		if(err != -EWOULDBLOCK)
			SetPageError(page);
		kunmap(page);
		kfree(in);
	}
	return err;
}

static int fuse_writepage(struct page *page, struct writeback_control *wbc)
{
	int err;
	struct inode *inode = page->mapping->host;
	unsigned count = get_write_count(inode, page);

	err = -EINVAL;
	if(count) {
		/* FIXME: check sync_mode, and wait for previous writes (or
		   signal userspace to do this) */
		if(wbc->nonblocking) {
			TestSetPageWriteback(page);
			err = write_buffer_nonblock(inode, page, 0, count);
			if (err)
				TestClearPageWriteback(page);
			if(err == -EWOULDBLOCK) {
				__set_page_dirty_nobuffers(page);
				err = 0;
			}
		} else
			err = write_buffer(inode, page, 0, count);
	}

	unlock_page(page);
	return err;
}

static int fuse_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;

	*pagep = grab_cache_page_write_begin(mapping, index, flags);
	if (!*pagep)
		return -ENOMEM;
	return 0;
}

static int fuse_buffered_write(struct file *file, struct inode *inode,
			       loff_t pos, unsigned count, struct page *page)
{
	int err;
	unsigned offset = pos & (PAGE_CACHE_SIZE - 1);

	if (is_bad_inode(inode))
		return -EIO;

	err = write_buffer(inode, page, offset, count);
	if (!err) {
		pos += count;
		if(pos > i_size_read(inode))
			i_size_write(inode, pos);
		if (count == PAGE_CACHE_SIZE)
			SetPageUptodate(page);
	}
	return err ? err : count;
}

static int fuse_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = mapping->host;
	int res = 0;

	if (copied)
		res = fuse_buffered_write(file, inode, pos, copied, page);

	unlock_page(page);
	page_cache_release(page);
	return res;
}


static struct file_operations fuse_file_operations = {
	.llseek		= generic_file_llseek,
	.read		= fuse_file_read,
	.aio_read	= generic_file_aio_read,
	.write		= do_sync_write,
	.aio_write	= generic_file_aio_write,
	.mmap		= generic_file_mmap,
	.open		= fuse_open,
	.release	= fuse_release,
	.fsync		= fuse_fsync,
	.splice_read	= generic_file_splice_read,
};

static struct address_space_operations fuse_file_aops  = {
	.readpage	= fuse_readpage,
	.writepage	= fuse_writepage,
	.write_begin	= fuse_write_begin,
	.write_end	= fuse_write_end,
};

void fuse_init_file_inode(struct inode *inode)
{
	struct fuse_conn *fc = INO_FC(inode);
	/* Readahead somehow defeats big reads on 2.6 (says Michael
           Grigoriev) */
	if(fc->flags & FUSE_LARGE_READ)
		inode->i_mapping->backing_dev_info->ra_pages = 0;
	inode->i_fop = &fuse_file_operations;
	inode->i_data.a_ops = &fuse_file_aops;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * c-basic-offset: 8
 * End:
 */
