/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2004  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#include "fuse_i.h"

#include <linux/init.h>
#include <linux/slab.h>

MODULE_AUTHOR("Miklos Szeredi <miklos@szeredi.hu>");
MODULE_DESCRIPTION("Filesystem in Userspace");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

spinlock_t fuse_lock = SPIN_LOCK_UNLOCKED;

/* Must be called with the fuse lock held */
void release_conn(struct fuse_conn *fc)
{
	cofs_volumes[fc->cofs_unit] = NULL;
	kfree(fc);
}

int __init cofuse_init(void)
{
	int res;

	printk(KERN_DEBUG "cofuse init %s (API version %i.%i)\n",
	       FUSE_VERSION,
	       FUSE_KERNEL_VERSION, FUSE_KERNEL_MINOR_VERSION);

	res = fuse_fs_init();
	if(res)
		goto err;

	res = fuse_dev_init();
	if(res)
		goto err_fs_cleanup;

	return 0;

  err_fs_cleanup:
	fuse_fs_cleanup();
  err:
	return res;
}

void __exit cofuse_exit(void)
{
	printk(KERN_DEBUG "cofuse exit\n");

	fuse_fs_cleanup();
	fuse_dev_cleanup();
}

module_init(cofuse_init);
module_exit(cofuse_exit);

/*
 * Local Variables:
 * indent-tabs-mode: t
 * c-basic-offset: 8
 * End:
 */
