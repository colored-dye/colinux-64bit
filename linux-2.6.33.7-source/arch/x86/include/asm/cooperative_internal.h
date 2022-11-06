/*
 *  linux/include/asm/cooperative_internal.h
 *
 *  Copyright (C) 2004 Dan Aloni
 */

#ifndef __LINUX_ASM_COOPERATIVE_INTERNAL_H__
#define __LINUX_ASM_COOPERATIVE_INTERNAL_H__

#include <asm/ptrace.h>

#ifdef CONFIG_COOPERATIVE

extern bool co_host_fpu_saved;
extern char co_host_fpu[0x200];

extern void co_kernel_breakpoint(struct pt_regs * regs);
extern int co_kernel_debug(struct pt_regs * regs, long error_code, unsigned int condition);

#define CO_FPU_SAVE(x) \
do \
{ \
	if (cpu_has_fxsr) \
		asm("fxsave " #x " ; fnclex"); \
	else \
		asm("fnsave " #x " ; fwait"); \
} \
while (0)

#define CO_FPU_RESTORE(x) \
do \
{ \
	if (cpu_has_fxsr) \
		asm("fxrstor " #x); \
	else \
		asm("frstor " #x); \
} \
while (0)

#else

static inline void co_kernel_breakpoint(struct pt_regs * regs)
{
}

static inline int co_kernel_debug(struct pt_regs * regs, long error_code, unsigned int condition)
{
	return 0;
}

#endif

#endif
