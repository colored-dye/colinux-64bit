/*
 *	Delay Loops for coLinux
 *
 *	The __delay function is not very precise under coLinux.
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timex.h>
#include <linux/preempt.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/processor.h>
#include <asm/delay.h>
#include <asm/timer.h>

void __delay(unsigned long loops)
{
	struct timespec tv;

	/*
	 * A bogos delay loop for creating BogoMIPS...
	 */

	loops = loops / 0x10000;
	while (loops--)
		read_persistent_clock(&tv);
}
EXPORT_SYMBOL(__delay);

void __const_udelay(unsigned long xloops)
{
	int d0;

	xloops *= 4;
	asm("mull %%edx"
		:"=d" (xloops), "=&a" (d0)
		:"1" (xloops), "0"
		(cpu_data(raw_smp_processor_id()).loops_per_jiffy * (HZ/4)));

	__delay(++xloops);
}
EXPORT_SYMBOL(__const_udelay);
