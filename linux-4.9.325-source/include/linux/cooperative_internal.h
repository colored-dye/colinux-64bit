/*
 *  linux/include/linux/cooperative_internal.h
 *
 *  Copyright (C) 2004 Dan Aloni
 *
 *  This header gathers the functions and variables in Cooperative Mode
 *  when CONFIG_COOPERATIVE is defined.
 */
#ifndef __LINUX_COOPERATIVE_LINUX_H__
#define __LINUX_COOPERATIVE_LINUX_H__

#include <linux/cooperative.h>
#include <linux/list.h>
#include <asm/ptrace.h>

#ifdef CONFIG_COOPERATIVE

#define ENABLE_PASSAGE_HOLDING_CHECK

typedef struct {
	struct list_head node;
	co_message_t msg;
} co_message_node_t;

extern co_boot_params_t co_boot_params;
#ifdef ENABLE_PASSAGE_HOLDING_CHECK
extern int co_passage_page_holding_count;
#endif

#ifdef CONFIG_COLINUX_STATS
typedef struct co_proc_counts {
	unsigned long switches[CO_OPERATION_MAX];
} co_proc_counts_t;

extern co_proc_counts_t co_proc_counts;
#endif

#define co_io_buffer ((co_io_buffer_t *)CO_VPTR_IO_AREA_START)
#define cooperative_mode_enabled()     1

extern void co_debug(const char *fmt, ...)
	__attribute__ ((format (printf, 1, 2)));
extern void co_printk(const char *line, int size);

extern void co_switch_wrapper(void);
extern void co_callback(struct pt_regs *regs);
extern void co_idle_processor(void);
NORET_TYPE void co_terminate(co_termination_reason_t reason) ATTRIB_NORET;
NORET_TYPE void co_terminate_panic(const char *text, int len) ATTRIB_NORET;
NORET_TYPE void co_terminate_bug(int code, int line, const char *file) ATTRIB_NORET;
extern void co_free_pages(unsigned long vaddr, int pages);
extern int co_alloc_pages(unsigned long vaddr, int pages);
extern void co_start_kernel(void);
extern void co_arch_start_kernel(void);

extern void co_send_message(co_module_t from,
			    co_module_t to,
			    co_priority_t priority,
			    co_message_type_t type,
			    unsigned long size,
			    const char *data);

extern int co_get_message(co_message_node_t **message, co_device_t device);
static inline void co_free_message(co_message_node_t *message)
{
	kfree(message);
}

extern void *co_map_buffer(void *, int);

static inline void co_passage_page_ref_up(void)
{
#ifdef ENABLE_PASSAGE_HOLDING_CHECK
	co_passage_page_holding_count++;
#endif
}

static inline void co_passage_page_ref_down(void)
{
#ifdef ENABLE_PASSAGE_HOLDING_CHECK
	co_passage_page_holding_count--;
#endif
}

static inline int co_passage_page_held(void)
{
#ifdef ENABLE_PASSAGE_HOLDING_CHECK
	return co_passage_page_holding_count;
#else
	return 0;
#endif
}

static inline void co_passage_page_acquire(unsigned long *flags)
{
	local_irq_save(*flags);
	co_passage_page_ref_up();
}

static inline void co_passage_page_release(unsigned long flags)
{
	co_passage_page_ref_down();
	local_irq_restore(flags);
}

#ifdef ENABLE_PASSAGE_HOLDING_CHECK
#define co_passage_page_assert_valid() do {	\
	BUG_ON(co_passage_page_held());		\
} while (0)
#else
#define co_passage_page_assert_valid() /* nothing */
#endif

static inline co_message_t *co_send_message_save(unsigned long *flags)
{
	co_passage_page_assert_valid();
	co_passage_page_acquire(flags);

	if (co_io_buffer->messages_waiting) {
		co_passage_page_release(*flags);
		return NULL;
	}

	co_passage_page->operation = CO_OPERATION_MESSAGE_TO_MONITOR;
	co_io_buffer->messages_waiting = 1;
	return ((co_message_t *)co_io_buffer->buffer);
}

static inline void co_send_message_restore(unsigned long flags)
{
	co_switch_wrapper();
	co_passage_page_release(flags);
}

#else

#define co_printk(line, size)          do {} while (0)
#define co_terminate(reason)           do {} while (0)
#define cooperative_mode_enabled()     0

#endif

#endif
