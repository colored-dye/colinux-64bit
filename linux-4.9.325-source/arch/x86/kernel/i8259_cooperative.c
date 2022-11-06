/*
 *  linux/arch/x86/kernel/i8259_cooperative.c
 */

#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/cooperative_internal.h>

#include <asm/irq.h>
#include <asm/desc.h>

CO_TRACE_STOP;

/* Function must have the same prototype as do_IRQ() */
unsigned int /*__irq_entry*/ proxy_interrupt_handler(struct pt_regs *regs)
{
	/* high bit used in ret_from_ code  */
	unsigned vector = ~regs->orig_ax;

	co_passage_page_assert_valid();
	co_passage_page_ref_up();
	co_passage_page->operation = CO_OPERATION_FORWARD_INTERRUPT;
	co_passage_page->params[0] = vector; /* IRQ vector */
	co_passage_page->host_state.flags &= ~(1 << 9); /* Turn IF off */
	co_switch_wrapper();
	co_callback(regs);
	return 1;
}

CO_TRACE_CONTINUE;

/*
 * Not like you have any other choice other than using
 * COPIC in Cooperative mode.
 */

static void end_COPIC_irq(unsigned int irq)
{
}

static void mask_and_ack_COPIC(unsigned int irq)
{
}

static unsigned int startup_COPIC_irq(unsigned int irq)
{
	return 0;
}

#define shutdown_COPIC_irq	disable_COPIC_irq

static void disable_COPIC_irq(unsigned int irq)
{
}

static void enable_COPIC_irq(unsigned int irq)
{
}

static struct irq_chip co_pic_irq_chip = {
	.name = "COPIC",
	.startup = startup_COPIC_irq,
	.shutdown = shutdown_COPIC_irq,
	.enable = enable_COPIC_irq,
	.disable = disable_COPIC_irq,
	.mask_ack = mask_and_ack_COPIC,
	.end = end_COPIC_irq,
	.set_affinity = NULL
};

void __init init_ISA_irqs (void)
{
	int i;

	printk("Setting proxy interrupt vectors (0..%d)\n", NR_IRQS-1);
	for (i = 0; i < NR_IRQS; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;

		set_irq_chip_and_handler_name(i, &co_pic_irq_chip, handle_simple_irq, "CO");
	}

}

void __init init_IRQ(void)
{
	int i;

	/* all the set up before the call gates are initialised */
	init_ISA_irqs();

	/*
	 * Cover the whole vector space, no vector can escape us.
	 */
	for (i = 0; i < (NR_VECTORS - FIRST_EXTERNAL_VECTOR); i++) {
		int vector = FIRST_EXTERNAL_VECTOR + i;
		if (i >= NR_IRQS)
			break;
		if (vector != SYSCALL_VECTOR)
			set_intr_gate(vector, interrupt[i]);
	}
}
