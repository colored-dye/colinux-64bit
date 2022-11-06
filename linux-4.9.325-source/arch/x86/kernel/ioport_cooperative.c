/*
 *	linux/arch/x86/kernel/ioport_cooperative.c
 */

#include <linux/syscalls.h>

asmlinkage long sys_ioperm(unsigned long from, unsigned long num, int turn_on)
{
	return -EPERM;
}

asmlinkage long sys_iopl(unsigned long unused)
{
	return -EPERM;
}
