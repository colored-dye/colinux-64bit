#ifndef _ASM_X86_PAGE_H
#define _ASM_X86_PAGE_H

#include <linux/types.h>

#ifdef __KERNEL__

#include <asm/page_types.h>

#ifdef CONFIG_X86_64
#include <asm/page_64.h>
#else
#include <asm/page_32.h>
#endif	/* CONFIG_X86_64 */

#ifndef __ASSEMBLY__
#include <asm/cooperative.h>

struct page;

static inline void clear_user_page(void *page, unsigned long vaddr,
				   struct page *pg)
{
	clear_page(page);
}

static inline void copy_user_page(void *to, void *from, unsigned long vaddr,
				  struct page *topage)
{
	copy_page(to, from);
}

#define __alloc_zeroed_user_highpage(movableflags, vma, vaddr) \
	alloc_page_vma(GFP_HIGHUSER | __GFP_ZERO | movableflags, vma, vaddr)
#define __HAVE_ARCH_ALLOC_ZEROED_USER_HIGHPAGE

#define __pa(x)		__phys_addr((unsigned long)(x))
#define __pa_nodebug(x)	__phys_addr_nodebug((unsigned long)(x))
/* __pa_symbol should be used for C visible symbols.
   This seems to be the official gcc blessed way to do such arithmetic. */
#define __pa_symbol(x)	__pa(__phys_reloc_hide((unsigned long)(x)))

#define __va(x)			((void *)((unsigned long)(x)+PAGE_OFFSET))

#define __boot_va(x)		__va(x)
#define __boot_pa(x)		__pa(x)

/*
 * virt_to_page(kaddr) returns a valid pointer if and only if
 * virt_addr_valid(kaddr) returns true.
 */
#define virt_to_page(kaddr)	pfn_to_page(__pa(kaddr) >> PAGE_SHIFT)
#define pfn_to_kaddr(pfn)      __va((pfn) << PAGE_SHIFT)
extern bool __virt_addr_valid(unsigned long kaddr);
#define virt_addr_valid(kaddr)	__virt_addr_valid((unsigned long) (kaddr))

#ifdef CONFIG_COOPERATIVE
#define CO_PA(pfn)		(((unsigned long *)CO_VPTR_PSEUDO_RAM_PAGE_TABLES)[pfn])
#define CO_VA_PFN(pa)		(((unsigned long *)CO_VPTR_PHYSICAL_TO_PSEUDO_PFN_MAP)[((pa) >> PAGE_SHIFT)])
#define CO_PFN_PP_TO_P(pfn)	(CO_PA(pfn) >> PAGE_SHIFT)
#define CO_PFN_P_TO_PP(pfn)	(CO_VA_PFN(pfn << PAGE_SHIFT))
#define CO_PP_TO_P(pa)	        ((CO_PFN_PP_TO_P(pa >> PAGE_SHIFT) << PAGE_SHIFT) | (pa & ~PAGE_MASK))
#define CO_P_TO_PP(pa)	        ((CO_PFN_P_TO_PP(pa >> PAGE_SHIFT) << PAGE_SHIFT) | (pa & ~PAGE_MASK))
#else
#define CO_PFN_P_TO_PP(pfn)	pfn
#define CO_PFN_PP_TO_P(pfn)	pfn
#define CO_PP_TO_P(pa)        	pa
#define CO_P_TO_PP(pa)	        pa
#endif

#endif	/* __ASSEMBLY__ */

#include <asm-generic/memory_model.h>
#include <asm-generic/getorder.h>

#define __HAVE_ARCH_GATE_AREA 1

#endif	/* __KERNEL__ */
#endif /* _ASM_X86_PAGE_H */
