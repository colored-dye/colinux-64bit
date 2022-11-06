/*
 *  linux/arch/x86/kernel/reboot_cooperative.c
 */

#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>
#include <linux/interrupt.h>
#include <linux/cooperative_internal.h>

/* sys_reboot needs this dummy. */
void (*pm_power_off)(void);

/*
 * This is a hack to make ctrl_alt_del work as a poweroff, so the OS can be
 * notified, unmount and sync everything.
 */
static int co_powering_off;

static int co_powerd(void *__unused)
{
	extern char * envp_init[];
	static char *const argv_shutdown[]= { "/sbin/shutdown", "-h", "now", NULL };
	static char *const argv_halt[]= { "/sbin/halt", NULL };

	if (kernel_execve(argv_shutdown[0], argv_shutdown, envp_init) >= 0)
		return 0;
	printk(KERN_INFO "%s spawn failed\n", argv_shutdown[0]);

	if (kernel_execve(argv_halt[0], argv_halt, envp_init) >= 0)
		return 0;
	printk(KERN_INFO "%s spawn failed\n", argv_halt[0]);

	printk(KERN_INFO "Fallback into reboot and power off\n");
	co_powering_off++;
	ctrl_alt_del();

	return 0;
}

static void deferred_shutdown(struct work_struct *dummy)
{
	kthread_run(co_powerd, NULL, "copowerd");
}

/*
 * This function will call from interrupt context.
 */
static void co_shedule_shutdown(void)
{
	static DECLARE_WORK(shutdown_work, deferred_shutdown);

	schedule_work(&shutdown_work);
}


static void co_machine_restart(void)
{
	co_terminate(CO_TERMINATE_REBOOT);
}

void machine_power_off(void)
{
	co_terminate(CO_TERMINATE_POWEROFF);
}

void machine_shutdown(void)
{
	co_terminate(CO_TERMINATE_HALT);
}

void machine_emergency_restart(void)
{
	co_machine_restart();
}

void machine_restart(char *cmd)
{
	if (co_powering_off)
		machine_power_off();
	else
		co_machine_restart();
}

void machine_halt(void)
{
	co_terminate(CO_TERMINATE_HALT);
}

static irqreturn_t power_interrupt(int irq, void *dev_id)
{
	co_message_node_t *node_message;

	if (co_get_message(&node_message, CO_DEVICE_POWER)) {
		co_linux_message_t *message;
		co_linux_message_power_t *type;

		message = (co_linux_message_t *)&node_message->msg.data;
		type = (co_linux_message_power_t *)message->data;
		switch (type->type) {
		case CO_LINUX_MESSAGE_POWER_ALT_CTRL_DEL:
			ctrl_alt_del();
			break;
		case CO_LINUX_MESSAGE_POWER_SHUTDOWN:
			co_shedule_shutdown();
			break;
		case CO_LINUX_MESSAGE_POWER_OFF:
			machine_power_off();
			break;
		default:
			printk(KERN_ERR "power interrupt: buggy type %d\n", type->type);
		}
		co_free_message(node_message);
	}

	return IRQ_HANDLED;
}

static int __init co_power_init(void)
{
	int rc;

	rc = request_irq(POWER_IRQ, &power_interrupt, IRQF_SAMPLE_RANDOM, "power", NULL);
	if (rc) {
		printk(KERN_ERR "POWER: unable to get irq %d", POWER_IRQ);
		return rc;
	}

	return 0;
}

__initcall(co_power_init);
