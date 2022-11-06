/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2004  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#include "fuse_i.h"

#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/file.h>

struct fuse_conn *cofs_volumes[CO_MODULE_MAX_COFS] = {NULL, };

static void cofuse_request_start(unsigned long *flags, struct fuse_conn *fc, struct fuse_in *in)
{
	co_passage_page_assert_valid();

	co_passage_page_acquire(flags);
	co_passage_page->operation = CO_OPERATION_DEVICE;
	co_passage_page->params[0] = CO_DEVICE_FILESYSTEM;
	co_passage_page->params[1] = fc->cofs_unit;
	co_passage_page->params[2] = in->h.opcode;
	co_passage_page->params[3] = in->h.ino;
	co_passage_page->params[4] = 0;
}

static void cofuse_request_end(unsigned long flags, struct fuse_out *out)
{
	unsigned long ret;
	ret = co_passage_page->params[4];
	co_passage_page_release(flags);
	out->h.error = ret;
}

void request_send(struct fuse_conn *fc, struct fuse_in *in,
		  struct fuse_out *out)
{
	unsigned long flags;
	char *str;

	switch ((unsigned long)in->h.opcode) {
	case FUSE_STATFS: {
		struct fuse_statfs_out *arg;

		arg = (struct fuse_statfs_out *)out->args[0].value;

		cofuse_request_start(&flags, fc, in);
		co_switch_wrapper();
		*arg = *(struct fuse_statfs_out *)&co_passage_page->params[5];
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_OPEN: {
		struct fuse_open_in *opin = (struct fuse_open_in *)in->args[0].value;

		cofuse_request_start(&flags, fc, in);
		co_passage_page->params[5] = opin->flags;
		co_switch_wrapper();
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_WRITE: {
		struct fuse_write_in *write_in = (struct fuse_write_in *)in->args[0].value;
		unsigned long long *offset_passage = (unsigned long long *)&co_passage_page->params[5];

		cofuse_request_start(&flags, fc, in);
		*offset_passage = write_in->offset;
		co_passage_page->params[7] = write_in->size;
		co_passage_page->params[8] = (unsigned long)in->args[1].value;
		co_switch_wrapper();
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_READ: {
		struct fuse_read_in *read_in = (struct fuse_read_in *)in->args[0].value;
		unsigned long long *offset_passage = (unsigned long long *)&co_passage_page->params[5];

		cofuse_request_start(&flags, fc, in);
		*offset_passage = read_in->offset;
		co_passage_page->params[7] = read_in->size;
		co_passage_page->params[8] = (unsigned long)out->args[0].value;
		co_switch_wrapper();
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_LOOKUP: {
		struct fuse_lookup_out *arg;

		arg = (struct fuse_lookup_out *)out->args[0].value;
		str = (char *)&co_passage_page->params[30];

		cofuse_request_start(&flags, fc, in);
		memcpy(str, (char *)in->args[0].value, in->args[0].size);
		co_switch_wrapper();
		*arg = *(struct fuse_lookup_out *)&co_passage_page->params[5];
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_RENAME: {
		struct fuse_rename_in *arg;
		char *str2;

		arg = (struct fuse_rename_in *)in->args[0].value;
		str = (char *)(&co_passage_page->params[30]);
		str2 = str + in->args[1].size;

		cofuse_request_start(&flags, fc, in);
		co_passage_page->params[5] = arg->newdir;
		memcpy(str, (char *)in->args[1].value, in->args[1].size);
		memcpy(str2, (char *)in->args[2].value, in->args[2].size);
		co_switch_wrapper();
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_MKNOD: {
		struct fuse_mknod_in *inarg;
		struct fuse_mknod_out *outarg;
		char *str;

		inarg = (struct fuse_mknod_in *)(in->args[0].value);
		outarg = (struct fuse_mknod_out *)(out->args[0].value);

		cofuse_request_start(&flags, fc, in);
		co_passage_page->params[5] = inarg->mode;
		co_passage_page->params[6] = inarg->rdev;
		str = (char *)&co_passage_page->params[30];
		memcpy(str, (char *)in->args[1].value, in->args[1].size);
		co_switch_wrapper();
		outarg->ino = co_passage_page->params[7];
		outarg->attr = *(struct fuse_attr *)(&co_passage_page->params[8]);
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_SETATTR: {
		struct fuse_setattr_in *inarg;
		struct fuse_setattr_out *outarg;
		struct fuse_attr *attr;

		inarg = (struct fuse_setattr_in *)(in->args[0].value);
		outarg = (struct fuse_setattr_out *)(out->args[0].value);
		attr = (struct fuse_attr *)(&co_passage_page->params[6]);

		cofuse_request_start(&flags, fc, in);
		co_passage_page->params[5] = inarg->valid;
		*attr = inarg->attr;
		co_switch_wrapper();
		outarg->attr = *attr;
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_MKDIR: {
		struct fuse_mkdir_in *arg;

		arg = (struct fuse_mkdir_in *)(in->args[0].value);
		str = (char *)&co_passage_page->params[30];

		cofuse_request_start(&flags, fc, in);
		co_passage_page->params[5] = arg->mode;
		memcpy(str, (char *)in->args[1].value, in->args[1].size);
		co_switch_wrapper();
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_UNLINK:
	case FUSE_RMDIR: {
		str = (char *)&co_passage_page->params[30];

		cofuse_request_start(&flags, fc, in);
		memcpy(str, (char *)in->args[0].value, in->args[0].size);
		co_switch_wrapper();
		cofuse_request_end(flags, out);
		return;
	}

	case FUSE_GETATTR: {
		struct fuse_getattr_out *arg;
		arg = (struct fuse_getattr_out *)out->args[0].value;

		co_passage_page_assert_valid();
		cofuse_request_start(&flags, fc, in);
		co_switch_wrapper();
		*arg = *(struct fuse_getattr_out *)&co_passage_page->params[5];
		cofuse_request_end(flags, out);
		return;
	}
	default: {
		static unsigned long warn_bit_array;
		unsigned long mask = 1 << in->h.opcode;

		if (!(warn_bit_array & mask)) {
			warn_bit_array |= mask;
			printk(KERN_WARNING "cofuse: unsuppored request %d\n", in->h.opcode);
		}
	}
	}

	out->h.error = -ENOSYS;
}

int request_send_nonblock(struct fuse_conn *fc, struct fuse_in *in,
			  struct fuse_out *out, fuse_reqend_t end, void *data)
{
	/* printk("cofuse: request_send_nonblock %d\n", in->h.opcode); */
	request_send(fc, in, out);
	end(fc, in, out, data);
	return 0;
}

int fuse_dev_init()
{
	return 0;
}

void fuse_dev_cleanup()
{
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * c-basic-offset: 8
 * End:
 */
