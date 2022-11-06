/*
 *  linux/kernel/cooperative.c
 *
 *  Cooperative mode (coLinux) support routines.
 *
 *  Dan Aloni <da-x@colinux.org>, 2003-2004 (C).
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/irq.h>
#include <linux/cooperative_internal.h>

CO_TRACE_STOP;

co_boot_params_t co_boot_params;

typedef struct {
	struct list_head list;
	int num_messages;
} co_message_queue_t;

int co_messages_active = 0;
co_message_queue_t co_outgoing_messages;
co_message_queue_t co_incoming_messages;
co_message_queue_t *co_incoming_queued_messages;

#ifdef CONFIG_COLINUX_STATS
co_proc_counts_t co_proc_counts;
#endif

void __init co_start_kernel(void)
{
        memcpy(&co_boot_params, co_passage_page->params, sizeof(co_boot_params));
	co_arch_start_kernel();
}

void co_send_message(co_module_t from,
		     co_module_t to,
		     co_priority_t priority,
		     co_message_type_t type,
		     unsigned long size,
		     const char *data)
{
	unsigned long flags;
	co_message_t *buffer;

	BUG_ON((sizeof(co_message_t) + size) > CO_VPTR_IO_AREA_SIZE - sizeof(co_io_buffer_t));

	buffer = co_send_message_save(&flags);
	if (!buffer)
		return;

	buffer->from = from;
	buffer->to = to;
	buffer->priority = priority;
	buffer->type = type;
	buffer->size = size;
	memcpy(buffer->data, data, size);

	co_send_message_restore(flags);
}

static void co_message_add_to_incoming(co_message_t *message, unsigned long size)
{
	co_message_node_t *message_copy;

	message_copy = kmalloc(size + sizeof(co_message_node_t) - sizeof(co_message_t),
			       GFP_ATOMIC);
	if (!message_copy)
		return;

	memcpy(&message_copy->msg, message, size);
	list_add_tail(&message_copy->node, &co_incoming_messages.list);
}

static void co_handle_jiffies(long count)
{
	if (count > HZ) {
		long secs = count / HZ;

		/* 'warp_clock' for long distances */
		write_seqlock(&xtime_lock);
		xtime.tv_sec += secs;
		count -= (secs * HZ);
		update_xtime_cache(0);
		write_sequnlock(&xtime_lock);
		clock_was_set();
	}

	while (count > 0) {
		irq_enter();
		__do_IRQ(TIMER_IRQ);
		irq_exit();
		count--;
	}
}

/* called with disabled interrupts */
static void co_handle_incoming_message(co_message_node_t *node_message)
{
	co_linux_message_t *message;
	co_message_queue_t *queue;
	int irq;

	message = (co_linux_message_t *)&node_message->msg.data;
	switch (message->device) {
	case CO_DEVICE_POWER: irq = POWER_IRQ; break;
	case CO_DEVICE_KEYBOARD: irq = KEYBOARD_IRQ; break;
#ifdef CONFIG_CONET_COOPERATIVE
	case CO_DEVICE_NETWORK: irq = NETWORK_IRQ; break;
#endif
#ifdef CONFIG_SERIAL_COOPERATIVE
	case CO_DEVICE_SERIAL: irq = SERIAL_IRQ; break;
#endif
	case CO_DEVICE_SCSI: irq = SCSI_IRQ; break;
	case CO_DEVICE_MOUSE: irq = MOUSE_IRQ; break;
	case CO_DEVICE_BLOCK: irq = BLOCKDEV_IRQ; break;
	default:
		BUG_ON((unsigned long)message->device >= (unsigned long)CO_DEVICES_TOTAL);
		co_free_message(node_message);
		return;
	}

	/* Add to the queue */
	queue = &co_incoming_queued_messages[message->device];
	list_add(&node_message->node, &queue->list);
	queue->num_messages++;

	irq_enter();
	__do_IRQ(irq);
	irq_exit();
}

static void co_handle_incoming_messages(void)
{
	if (!co_messages_active)
		return;

	/*
	 * Pop a message from the incoming queue.
	 */
	while (!list_empty(&co_incoming_messages.list)) {
		co_message_node_t *message;

		message = list_entry(co_incoming_messages.list.next,
				     co_message_node_t, node);
		BUG_ON((unsigned long)message->msg.from >= (unsigned long)CO_MODULES_MAX);
		BUG_ON((unsigned long)message->msg.to >= (unsigned long)CO_MODULES_MAX);
		list_del(&message->node);

		/*
		 * Let the interrupt routine of the arch dependant code
		 * handle the message, and be responsible to free it.
		 */
		co_handle_incoming_message(message);
	}
}

void co_callback(struct pt_regs *regs)
{
	long io_size;
	unsigned long new_jiffies;
	struct pt_regs null_regs;

	BUG_ON(!irqs_disabled());
	if (co_passage_page->operation != CO_OPERATION_MESSAGE_FROM_MONITOR) {
		co_passage_page_ref_down();
		return;
	}

#ifdef CONFIG_COLINUX_STATS
	co_proc_counts.switches[CO_OPERATION_MESSAGE_FROM_MONITOR]++;
#endif
	io_size = co_passage_page->params[0];
	new_jiffies = co_passage_page->params[1];

	if (co_messages_active  &&  io_size > 0  &&  io_size <= CO_VPTR_IO_AREA_SIZE) {
		static unsigned char temp_storage[CO_VPTR_IO_AREA_SIZE];
		unsigned char *io_buffer = temp_storage;
		unsigned char *io_buffer_end = &temp_storage[io_size];

		/* Copy into temp, because kmalloc calls host to mapping pages */
		memcpy(temp_storage, co_io_buffer->buffer, io_size);
		co_io_buffer->messages_waiting = 0;
		co_passage_page_ref_down();

		while (io_buffer < io_buffer_end) {
			co_message_t *message = (co_message_t *)io_buffer;
			co_linux_message_t *linux_message = (co_linux_message_t *)message->data;
			unsigned long size = message->size + sizeof(*message);

			BUG_ON((unsigned long)message->from >= (unsigned long)CO_MODULES_MAX);
			BUG_ON((unsigned long)message->to >= (unsigned long)CO_MODULES_MAX);
			BUG_ON((unsigned long)linux_message->device >= (unsigned long)CO_DEVICES_TOTAL);

			co_message_add_to_incoming(message, size);
			io_buffer += size;
		}
	} else {
		co_io_buffer->messages_waiting = 0;
		co_passage_page_ref_down();
	}

	memset (&null_regs, 0, sizeof(null_regs));

	/* Have only, if from proxy_interrupt_handler(), needs for user_mode() */
	if (regs)
		null_regs.cs = regs->cs;
	set_irq_regs(&null_regs);

	co_handle_jiffies(new_jiffies);
	co_handle_incoming_messages();
}

void co_idle_processor(void)
{
	co_passage_page_assert_valid();
	local_irq_disable();
	co_passage_page_ref_up();
	co_passage_page->operation = CO_OPERATION_IDLE;
	co_switch_wrapper();
	co_callback(NULL);
	local_irq_enable();
}

void co_printk(const char *line, int size)
{
	unsigned long flags;
	co_message_t *co_message;

	co_message = co_send_message_save(&flags);
	if (co_message) {
		co_message->from = CO_MODULE_LINUX;
		co_message->to = CO_MODULE_PRINTK;
		co_message->priority = CO_PRIORITY_DISCARDABLE;
		co_message->type = CO_MESSAGE_TYPE_STRING;
		if (size > 200)
			size = 200;
		co_message->size = size + 1;
		memcpy(co_message->data, line, size);
		co_message->data[size] = '\0';
		co_send_message_restore(flags);
	}
}

NORET_TYPE void co_terminate_panic(const char *text, int len)
{
	unsigned long flags;

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_TERMINATE;
	co_passage_page->params[0] = CO_TERMINATE_PANIC;
	co_passage_page->params[1] = 0;
	co_passage_page->params[2] = 0;
	co_passage_page->params[3] = len;
	memcpy((char *)&co_passage_page->params[4], text, len+1);
	co_switch_wrapper();
	while(1);
}

NORET_TYPE void co_terminate_bug(int code, int line, const char *file)
{
	unsigned long flags;

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_TERMINATE;
	co_passage_page->params[0] = CO_TERMINATE_BUG;
	co_passage_page->params[1] = code;
	co_passage_page->params[2] = line;
	co_passage_page->params[3] = strlen(file);
	strcpy((char *)&co_passage_page->params[4], file);
	co_switch_wrapper();
	while(1);
}
NORET_TYPE EXPORT_SYMBOL(co_terminate_bug);

void co_terminate(co_termination_reason_t reason)
{
	unsigned long flags;

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_TERMINATE;
	co_passage_page->params[0] = reason;
	co_passage_page->params[3] = 0; /* len */
	co_switch_wrapper();
	while(1);
}
EXPORT_SYMBOL(co_terminate);

int co_get_message(co_message_node_t **message, co_device_t device)
{
	co_message_queue_t *queue;
	co_message_node_t *node;
	unsigned long flags;

	if (!co_messages_active)
		return 0;

	local_irq_save(flags);
	queue = &co_incoming_queued_messages[device];
	if (list_empty(&queue->list)) {
		local_irq_restore(flags);
		return 0;
	}

	node = list_entry(queue->list.prev, co_message_node_t, node);
	list_del(&node->node);
	queue->num_messages--;
	local_irq_restore(flags);

	*message = node;
	return 1;
}

co_info_t co_info = {
	.api_version = CO_LINUX_API_VERSION,
	.compiler_major = __GNUC__,
	.compiler_minor = __GNUC_MINOR__,
	.compiler_abi = __GXX_ABI_VERSION,
};

static int __init initcall_message_queues(void)
{
	int queue_index;

	INIT_LIST_HEAD(&co_outgoing_messages.list);
	INIT_LIST_HEAD(&co_incoming_messages.list);

	co_incoming_queued_messages =
		kmalloc(sizeof(co_message_queue_t) * CO_DEVICES_TOTAL, GFP_KERNEL);
	if (!co_incoming_queued_messages)
		panic("unable to allocate message queues\n");

	for (queue_index=0; queue_index < CO_DEVICES_TOTAL; queue_index++) {
		co_message_queue_t *queue = &co_incoming_queued_messages[queue_index];
		queue->num_messages = 0;
		INIT_LIST_HEAD(&queue->list);
	}

	co_messages_active = 1;

	return 0;
}


void co_free_pages(unsigned long vaddr, int pages)
{
	unsigned long flags;

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_FREE_PAGES;
	co_passage_page->params[0] = vaddr;
	co_passage_page->params[1] = pages;
	co_switch_wrapper();
	co_passage_page_release(flags);
}

int co_alloc_pages(unsigned long vaddr, int size)
{
	unsigned long flags;
	long result;

	co_passage_page_acquire(&flags);
	co_passage_page->operation = CO_OPERATION_ALLOC_PAGES;
	co_passage_page->params[0] = vaddr;
	co_passage_page->params[1] = size;
	co_switch_wrapper();
	result = (long)co_passage_page->params[4];
	co_passage_page_release(flags);

	if (result < 0)
		return -ENOMEM;

	return 0;
}

__initcall(initcall_message_queues);

#ifdef CONFIG_COLINUX_STATS
static int co_guest_read_status(char *page, char **start, off_t off, int count, int *eof, void *clear)
{
	co_proc_counts_t hold;
	int len;

	local_irq_disable();
	hold = co_proc_counts;
	if (clear)
		memset(&co_proc_counts, 0, sizeof(co_proc_counts));
	local_irq_enable();

	len = sprintf(page,	"idle:\t%lu\n"
				"msgto:\t%lu\n"
				"mfrom:\t%lu\n"
				"intr:\t%lu\n"
				"dev:\t%lu\n"
				"time:\t%lu\n"
				"hpt:\t%lu\n"
				"free:\t%lu\n"
				"alloc:\t%lu\n"
				"getpp:\t%lu\n",
			hold.switches[CO_OPERATION_IDLE],
			hold.switches[CO_OPERATION_MESSAGE_TO_MONITOR],
			hold.switches[CO_OPERATION_MESSAGE_FROM_MONITOR],
			hold.switches[CO_OPERATION_FORWARD_INTERRUPT],
			hold.switches[CO_OPERATION_DEVICE],
			hold.switches[CO_OPERATION_GET_TIME],
			hold.switches[CO_OPERATION_GET_HIGH_PREC_TIME],
			hold.switches[CO_OPERATION_FREE_PAGES],
			hold.switches[CO_OPERATION_ALLOC_PAGES],
			hold.switches[CO_OPERATION_GETPP]);

	return len;
}

static __init int co_create_proc_stats(void)
{
	struct proc_dir_entry *co_guest_dir;

	co_guest_dir = proc_mkdir("colinux", NULL);
	if(co_guest_dir) {
		create_proc_read_entry("stats",
					0444, co_guest_dir,
					co_guest_read_status, NULL);
		create_proc_read_entry("stats_clear",
					0444, co_guest_dir,
					co_guest_read_status, (void*)1);
	}

	return 0;
}

__initcall(co_create_proc_stats);
#endif /* CONFIG_COLINUX_STATS */

CO_TRACE_CONTINUE;
