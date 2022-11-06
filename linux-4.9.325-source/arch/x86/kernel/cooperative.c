#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/reboot.h>

#include <linux/cooperative_internal.h>
#include <asm/cooperative_internal.h>
#include <asm/cpu.h>
#include <asm/processor.h>
#include <asm/desc.h>
#include <asm/mmu_context.h>
#include <asm/debugreg.h>
#include <asm/i387.h>

CO_TRACE_STOP;

#ifdef ENABLE_PASSAGE_HOLDING_CHECK
int co_passage_page_holding_count = 0;
#endif
bool co_host_fpu_saved = 0;
char co_host_fpu[0x200] __attribute__ ((__aligned__(16)));

static void __init co_early_cpu_init(void)
{
	/*
	 * On the first switch to Linux we must set up a valid TR because
	 * the passage page code assumes such one exists. This is basically
	 * copied code from cpu_init().
	 *
	 * P.S this is protected by CO_TRACE_STOP so that we don't
	 * have a monitor context switch.
	 */
	struct task_struct *me;
	struct tss_struct *t;
	int cpu;
	int i;
	struct desc_struct *gdt;

	cpu = stack_smp_processor_id();
	t = &per_cpu(init_tss, cpu);

	me = current;

	write_cr4(mmu_cr4_features);

	/*
	 * Initialize the per-CPU GDT with the boot GDT,
	 * and set up the GDT descriptor:
	 */

	switch_to_new_gdt(cpu);
	loadsegment(fs, 0);

	load_idt(&idt_descr);

	memset(me->thread.tls_array, 0, GDT_ENTRY_TLS_ENTRIES * 8);

	/*
	 * Delete NT
	 */
	__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");

	t->x86_tss.io_bitmap_base = offsetof(struct tss_struct, io_bitmap);

	/*
	 * <= is required because the CPU will access up to
	 * 8 bits beyond the end of the IO permission bitmap.
	 */
	for (i = 0; i <= IO_BITMAP_LONGS; i++)
		t->io_bitmap[i] = ~0UL;

	atomic_inc(&init_mm.mm_count);
	me->active_mm = &init_mm;
	enter_lazy_tlb(&init_mm, me);

	load_sp0(t, &current->thread);
	set_tss_desc(cpu, t);
	gdt = get_cpu_gdt_table(cpu);
	gdt[GDT_ENTRY_TSS].b &= 0xfffffdff;

	load_TR_desc();
	load_LDT(&init_mm.context);

	/* Clear %gs. */
	asm volatile ("mov %0, %%gs" : : "r" (0));
}

/*
 * 'co_start' is the first Linux code that runs in the
 * coLinux kernel context. It receives %ecx which contains the
 * address of the passage page. The passage page code sets %ecx
 * to this value in its context restore part.
 */

void __init co_start(void)
{
	co_early_cpu_init();
	co_start_kernel();
}

static void co_switch_wrapper_protected(void)
{
	kernel_fpu_begin();

	if (co_host_fpu_saved) {
		CO_FPU_RESTORE(co_host_fpu);
		co_host_fpu_saved = 0;
	}

	/* And switch... */
	co_switch();

	kernel_fpu_end();
}

void co_switch_wrapper(void)
{
	/* taken from irq.c: debugging check for stack overflow */
	long esp;

	__asm__ __volatile__("andl %%esp,%0" : "=r" (esp) : "0" (THREAD_SIZE - 1));
	if (unlikely(esp < (sizeof(struct thread_info) + STACK_WARN))) {
		printk("co_switch_wrapper: stack overflow: %ld\n", esp - sizeof(struct thread_info));
		co_terminate(CO_TERMINATE_STACK_OVERFLOW);
	}

#ifdef CONFIG_COLINUX_STATS
	co_proc_counts.switches[co_passage_page->operation]++;
#endif
	co_switch_wrapper_protected();
}

void co_debug(const char *fmt, ...)
{
}

#define MAX_TRACE_POINTS 1024

typedef struct {
	unsigned char *code;
	unsigned char original_byte;
	int off;
} co_tracepoint_t;

co_tracepoint_t tracepoints[MAX_TRACE_POINTS];
static int active_tracepoints = 0;

void co_kernel_breakpoint(struct pt_regs * regs)
{
	int i = 0;
	unsigned char *code = (unsigned char *)regs->ip;
	if (!code)
		return;

	code--;
	for (i=0; i < active_tracepoints; i++) {
		if (tracepoints[i].code == code) {
			co_debug("TRACEPOINT: %p", code);
			break;
		}
	}

	if (i == active_tracepoints) {
		/* Bad, we don't know this tracepoint */
		co_terminate(CO_TERMINATE_INVALID_OPERATION);
		return;
	}

	*tracepoints[i].code = tracepoints[i].original_byte;
	regs->flags |= (1 << 8); /* Enable TF */
	regs->ip = (unsigned long)code;
	tracepoints[i].off = 1;
}

void co_kernel_set_breakpoints(void)
{
	int i;

	for (i=0; i < active_tracepoints; i++)
		if (tracepoints[i].code  &&  tracepoints[i].off) {
			*tracepoints[i].code = 0xcc;
			tracepoints[i].off = 0;
		}
}

int co_kernel_debug(struct pt_regs *regs, long error_code, unsigned int condition)
{
	/* if not a single step trap */
	if (!(condition & DR_STEP))
		return 0;

	/* if userspace */
	if (regs->cs & 3)
		return 0;

	regs->flags &= ~(1 << 8); /* Disable TF */

	co_kernel_set_breakpoints();

	return 1;
}

void co_kernel_tracepoint_add(unsigned char *code)
{
	if (active_tracepoints >= MAX_TRACE_POINTS)
		return;

	tracepoints[active_tracepoints].code = code;
	tracepoints[active_tracepoints].original_byte = *code;
	tracepoints[active_tracepoints].off = 0;
	active_tracepoints++;
	*code = 0xcc;
}

co_arch_info_t co_arch_info = {
	.kernel_cs = __KERNEL_CS,
	.kernel_ds = __KERNEL_DS,
};

CO_TRACE_CONTINUE;
