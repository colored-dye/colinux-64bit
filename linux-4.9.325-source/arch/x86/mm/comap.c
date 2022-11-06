
/*
 * This source code is a part of coLinux source package.
 *
 * Copyright (C) 2008 Steve Shoecraft <sshoecraft@earthlink.net>
 *
 * The code is licensed under the GPL.  See the COPYING file in
 * the root directory.
 *
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>

#include <linux/cooperative.h>
#include <linux/cooperative_internal.h>

static unsigned long *get_pp(void *host_buffer, int size) {
	unsigned long *pp, flags;
	int npages,pp_size,rc;

	/* Get # of pages */
	npages = size >> PAGE_SHIFT;
	if ((npages * PAGE_SIZE) < size) npages++;

	/* Alloc mem for phys pages */
	pp_size = npages * sizeof(unsigned long);
	pp = kmalloc(pp_size, GFP_KERNEL);
	if (!pp) {
		printk(KERN_ERR "co_map_buffer: error allocating memory for physical pages!\n");
		return 0;
	}

	/* Request physical pages from the host */
	co_passage_page_assert_valid();
	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_GETPP;
	co_passage_page->params[0] = (unsigned long) pp;
	co_passage_page->params[1] = (unsigned long) host_buffer;
	co_passage_page->params[2] = size;
	co_switch_wrapper();
	rc = co_passage_page->params[0];
	co_passage_page_release(flags);
	if (rc) {
		printk(KERN_ERR "co_map_buffer: error getting physical pages from host!\n");
		return 0;
	}

	return pp;
}

#define VM_OPTS VM_READ | VM_WRITE | VM_MAYREAD | VM_MAYWRITE | VM_PFNMAP | VM_RAND_READ

void *co_map_buffer(void *host_buffer, int size) {
	unsigned long addr, *pp, *pd, *pt, pa;
	struct vm_struct *area;
	void *buffer;
	int i,npages,pd_index,pt_index;

	/* Get the physical pages for the host buffer */
	pp = get_pp(host_buffer, size);
	if (!pp) return 0;

	/* Get an area of virtual memory */
	area = get_vm_area(size, VM_OPTS);
	buffer = area->addr;

        npages = size >> PAGE_SHIFT;

	addr = (unsigned long) buffer;
	pd = (unsigned long *) init_mm.pgd;
	while(npages) {
		/* Get pt */
		pd_index = pgd_index(addr);
		if (pd[pd_index] != 0) {
			pa = pd[pd_index] & PAGE_MASK;
			pt = __va(CO_P_TO_PP(pa));
		} else {
			pt = (unsigned long *) kmalloc(PAGE_SIZE, GFP_KERNEL);
			if (!pt) {
				printk(KERN_ERR "co_map_buffer: unable to alloc new pt entry!\n");
				return 0;
			}
			memset(pt, 0, PAGE_SIZE);
			pa = CO_PP_TO_P(__pa(pt)) | _PAGE_TABLE;
			pd[pd_index] = pa;
		}

		/* Fill pt */
		pt_index = pte_index(addr);
		for(i=pt_index; i < 1024; i++) {
			if (!npages) break;
			BUG_ON(pt[i] != 0);
			pt[i] = *pp | _PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_DIRTY | _PAGE_ACCESSED;
			pp++;
			addr += PAGE_SIZE;
			npages--;
		}
	}

	return buffer;
}

#if 0
void dump_pt(void *buf, int size) {
	unsigned long addr, *pd, *pt;
	int x, pd_index, pt_index, npages;

	npages = size >> PAGE_SHIFT;

	addr = (unsigned long) buf;
	pd = (unsigned long *)  __va(read_cr3());
	printk(KERN_INFO "pd: %p\n", pd);
	while(npages) {
		pd_index = pgd_index(addr);
		printk(KERN_INFO "pd[%04d]: %lx\n", pd_index, pd[pd_index]);
		BUG_ON(pd[pd_index] == 0);
		pt = __va(CO_P_TO_PP(pd[pd_index] & PAGE_MASK));
		pt_index = pte_index(addr);
		for(x=pt_index; x < 1024; x++) {
			if (!npages) break;
			printk(KERN_INFO "%04d: pt[%04d]: %08lx\n", npages, x, pt[x]);
			addr += PAGE_SIZE;
			npages--;
		}
	}
}
#endif
