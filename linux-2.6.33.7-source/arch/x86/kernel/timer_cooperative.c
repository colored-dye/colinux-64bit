/*
 *  Cooperative mode timer.
 *
 *  Dan Aloni <da-x@colinux.org>, 2003-2004 (C).
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#include <asm/timer.h>
#include <asm/delay.h>

#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/cooperative.h>
#include <linux/cooperative_internal.h>

#define CO_CLOCK_SHIFT 20

/* FSEC = 10^-15
   NSEC = 10^-9 */
#define FSEC_PER_NSEC	1000000

/* 100Hz is fix from damons 10ms setup */
#define CO_HOST_HZ 100

unsigned int cpu_khz;
EXPORT_SYMBOL(cpu_khz);

/*
 * Functions copied from time.c
 */
unsigned long profile_pc(struct pt_regs *regs)
{
	unsigned long pc = instruction_pointer(regs);

	return pc;
}
EXPORT_SYMBOL(profile_pc);

/* Read system date and clock at startup */
void read_persistent_clock(struct timespec *ts)
{
	unsigned long flags;

	co_passage_page_assert_valid();

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_GET_TIME;
	co_switch_wrapper();
	ts->tv_sec = co_passage_page->params[0];
	ts->tv_nsec = 0;
	co_passage_page_release(flags);
}

/* need by ntp.c */
int update_persistent_clock(struct timespec now)
{
	return 0;
}


/*
 * Clock source related code, based on arch/arm/mach-omap1/time.c
 */

static cycle_t query_host_highprec_time(struct clocksource *cs)
{
	unsigned long flags;
	unsigned long long this_time;

	co_passage_page_assert_valid();

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_GET_HIGH_PREC_TIME;
	co_switch_wrapper();
	this_time = *(unsigned long long *)(&co_passage_page->params[0]);
	co_passage_page_release(flags);

	return this_time;
}

static struct clocksource co_clocksource = {
	.name		= "cooperative",
	.rating		= 450,
	.read		= query_host_highprec_time,
	.mask		= CLOCKSOURCE_MASK(64),
	.shift		= CO_CLOCK_SHIFT,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};


static void co_timer_setup(enum clock_event_mode mode,
			   struct clock_event_device *evt)
{
}

struct clock_event_device co_clockevent = {
	.name		= "clock",
	.features	= CLOCK_EVT_FEAT_PERIODIC,
	.set_mode	= co_timer_setup,
	.shift		= CO_CLOCK_SHIFT,
};

static irqreturn_t co_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = &co_clockevent;

	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static struct irqaction co_timer_irq = {
	.name		= "timer",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= co_timer_interrupt,
};

extern void (*late_time_init)(void);
static void __init co_register_timer(void)
{
	setup_irq(TIMER_IRQ, &co_timer_irq);
}

void __init time_init(void)
{
	u64 tmp;

	tmp = (u64)CO_HOST_HZ << CO_CLOCK_SHIFT;
	do_div(tmp, FSEC_PER_NSEC);
	co_clockevent.mult = (u32)tmp;

	co_clocksource.mult = clocksource_hz2mult(CO_HOST_HZ, CO_CLOCK_SHIFT);
	co_clockevent.cpumask = cpumask_of(0);

	clockevents_register_device(&co_clockevent);
	late_time_init = co_register_timer;

	cpu_khz = co_boot_params.co_cpu_khz;
}
