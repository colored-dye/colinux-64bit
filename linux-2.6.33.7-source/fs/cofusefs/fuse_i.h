/*
    COFUSE: Filesystem in an host of Cooperative Linux
    Copyright (C) 2004 Dan Aloni <da-x@colinux.org>

    based on FUSE: Filesystem in Userspace
    Copyright (C) 2001-2004  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/


#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0) && LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#error Kernel version 2.5.* not supported
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/cooperative_internal.h>
#include <linux/cooperative_fs.h>

/** Read combining parameters */
#define FUSE_BLOCK_SHIFT 16
#define FUSE_BLOCK_SIZE 65536
#define FUSE_BLOCK_MASK 0xffff0000

#define FUSE_BLOCK_PAGE_SHIFT (FUSE_BLOCK_SHIFT - PAGE_CACHE_SHIFT)

/**
 * A Fuse connection.
 *
 * This structure is created, when the client device is opened, and is
 * destroyed, when the client device is closed _and_ the filesystem is
 * unmounted.
 */
struct fuse_conn {
	/** The superblock of the mounted filesystem */
	struct super_block *sb;

	/** index on the host side **/
	int cofs_unit;

	/** The user id for this mount */
	uid_t uid;

	/** The fuse mount flags for this mount */
	unsigned int flags;

	/** Is the new (synchronous) release not supported by
	    userspace? */
	unsigned int oldrelease;

	char opt_pathname[0x80];
};

/** One input argument of a request */
struct fuse_in_arg {
	unsigned int size;
	const void *value;
};

/** The request input */
struct fuse_in {
	struct fuse_in_header h;
	unsigned int numargs;
	struct fuse_in_arg args[3];
};

/** One output argument of a request */
struct fuse_out_arg {
	unsigned int size;
	void *value;
};

/** The request output */
struct fuse_out {
	struct fuse_out_header h;
	unsigned int argvar;
	unsigned int numargs;
	struct fuse_out_arg args[3];
};

#define FUSE_IN_INIT { {0, 0, 0, current_fsuid(), current_fsgid()}, 0}
#define FUSE_OUT_INIT { {0, 0}, 0, 0}

struct fuse_req;
typedef void (*fuse_reqend_t)(struct fuse_conn *, struct fuse_in *,
			      struct fuse_out *, void *data);

/**
 * A request to the client
 */
struct fuse_req {
	/** The request list */
	struct list_head list;

	/** True if the request is synchronous */
	unsigned int issync:1;

	/** The request is locked */
	unsigned int locked:1;

	/** The request has been interrupted while it was locked */
	unsigned int interrupted:1;

	/* The request has been sent to the client */
	unsigned int sent:1;

	/* The request is finished */
	unsigned int finished:1;

	/** The request input */
	struct fuse_in *in;

	/** The request output */
	struct fuse_out *out;

	/** Used to wake up the task waiting for completion of request*/
	wait_queue_head_t waitq;

	/** Request completion callback */
	fuse_reqend_t end;

	/** User data */
	void *data;
};

#define SB_FC(sb) ((sb)->s_fs_info)
#define INO_FC(inode) SB_FC((inode)->i_sb)
#define DEV_FC(file) ((struct fuse_conn *) (file)->private_data)


/**
 * The proc entry for the client device ("/proc/fs/fuse/dev")
 */
extern struct proc_dir_entry *proc_fuse_dev;

/**
 * The lock to protect fuses structures
 */
extern spinlock_t cofuse_lock;


/**
 * Get a filled in inode
 */
struct inode *cofuse_iget(struct super_block *sb, ino_t ino,
	   	  	  struct fuse_attr *attr, int version);


/**
 * Initialise operations on regular file
 */
void cofuse_init_file_inode(struct inode *inode);

/**
 * Check if the connection can be released, and if yes, then free the
 * connection structure
 */
void cofuse_release_conn(struct fuse_conn *fc);

/**
 * Initialize the client device
 */
int cofuse_dev_init(void);

/**
 * Cleanup the client device
 */
void cofuse_dev_cleanup(void);

/**
 * Initialize the fuse filesystem
 */
int cofuse_fs_init(void);

/**
 * Cleanup the fuse filesystem
 */
void cofuse_fs_cleanup(void);

/**
 * Send a request
 *
 */
void cofuse_request_send(struct fuse_conn *fc, struct fuse_in *in,
    			 struct fuse_out *out);

/**
 * Send a synchronous request without blocking
 */
int cofuse_request_send_nonblock(struct fuse_conn *fc, struct fuse_in *in,
	  		         struct fuse_out *out, fuse_reqend_t end, void *data);

/**
 * Get the attributes of a file
 */
int cofuse_do_getattr(struct inode *inode);

/**
 * Write dirty pages
 */
void cofuse_sync_inode(struct inode *inode);

/*
 * Local Variables:
 * indent-tabs-mode: t
 * c-basic-offset: 8
 * End:
 */

#define COFUSE_VERSION "0.1"
#define FUSE_VERSION COFUSE_VERSION

#define fuse_init_file_inode cofuse_init_file_inode
#define fuse_do_getattr cofuse_do_getattr
#define fuse_sync_inode cofuse_sync_inode
#define fuse_lock cofuse_lock

#define request_send cofuse_request_send
#define request_send_nonblock cofuse_request_send_nonblock
#define release_conn cofuse_release_conn
#define fuse_iget cofuse_iget
#define fuse_dev_init cofuse_dev_init
#define fuse_dev_cleanup cofuse_dev_cleanup
#define fuse_fs_init cofuse_fs_init
#define fuse_fs_cleanup cofuse_fs_cleanup

extern struct fuse_conn *cofs_volumes[CO_MODULE_MAX_COFS];

/** Data passed to mount */
struct cofuse_mount_data {
	struct fuse_mount_data *fuse;
	int uid;
	int gid;
	unsigned long file_mode;
	unsigned long dir_mode;
	unsigned long flags;
	char name[0x80];
};
