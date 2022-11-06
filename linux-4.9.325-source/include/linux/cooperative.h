/*
 *  linux/include/linux/cooperative.h
 *
 *  Copyright (C) 2004 Dan Aloni
 *
 *  This file defines the interfaces between the Cooperative Linux kernel
 *  and the host OS driver. It's for both external inclusion from the
 *  and internal inclusion in the kernel sources.
 */

#ifndef __LINUX_COOPERATIVE_H__
#define __LINUX_COOPERATIVE_H__

#ifdef __KERNEL__
#ifndef CO_KERNEL
#define CO_COLINUX_KERNEL
#define CO_KERNEL
#endif
#endif

#include <asm/cooperative.h>

#define CO_LINUX_API_VERSION    14

#pragma pack(0)

#define CO_BOOTPARAM_STRING_LENGTH 0x100

typedef enum {
	CO_OPERATION_EMPTY=0,
	CO_OPERATION_START,
	CO_OPERATION_IDLE,
	CO_OPERATION_TERMINATE,
	CO_OPERATION_MESSAGE_TO_MONITOR,
	CO_OPERATION_MESSAGE_FROM_MONITOR,
	CO_OPERATION_FORWARD_INTERRUPT,
	CO_OPERATION_DEVICE,
	CO_OPERATION_GET_TIME,
	CO_OPERATION_DEBUG_LINE,
	CO_OPERATION_GET_HIGH_PREC_TIME,
	CO_OPERATION_TRACE_POINT,
	CO_OPERATION_FREE_PAGES,
	CO_OPERATION_ALLOC_PAGES,
	CO_OPERATION_PRINTK_unused,
	CO_OPERATION_GETPP,
	CO_OPERATION_MAX	/* Must be last entry all times */
} co_operation_t;

#define CO_MODULE_MAX_CONET    16
#define CO_MODULE_MAX_COBD     32
#define CO_MODULE_MAX_COFS     32
#define CO_MODULE_MAX_SERIAL   32
#define CO_MODULE_MAX_COSCSI   32
#define CO_MODULE_MAX_COVIDEO	2
#define CO_MODULE_MAX_COAUDIO	2

typedef enum {
	CO_MODULE_LINUX,
	CO_MODULE_MONITOR,
	CO_MODULE_DAEMON,
	CO_MODULE_IDLE,
	CO_MODULE_KERNEL_SWITCH,
	CO_MODULE_USER_SWITCH,
	CO_MODULE_CONSOLE,
	CO_MODULE_PRINTK,

	CO_MODULE_CONET0,
	CO_MODULE_CONET_END=CO_MODULE_CONET0+CO_MODULE_MAX_CONET-1,

	CO_MODULE_COBD0,
	CO_MODULE_COBD_END=CO_MODULE_COBD0+CO_MODULE_MAX_COBD-1,

	CO_MODULE_COFS0,
	CO_MODULE_COFS_END=CO_MODULE_COFS0+CO_MODULE_MAX_COFS-1,

	CO_MODULE_SERIAL0,
	CO_MODULE_SERIAL_END=CO_MODULE_SERIAL0+CO_MODULE_MAX_SERIAL-1,

	CO_MODULE_COSCSI0,
	CO_MODULE_COSCSI_END=CO_MODULE_COSCSI0+CO_MODULE_MAX_COSCSI-1,

	CO_MODULE_COVIDEO0,
	CO_MODULE_COVIDEO_END=CO_MODULE_COVIDEO0+CO_MODULE_MAX_COVIDEO-1,

	CO_MODULE_COAUDIO0,
	CO_MODULE_COAUDIO_END=CO_MODULE_COAUDIO0+CO_MODULE_MAX_COAUDIO-1,
	CO_MODULES_MAX,
} co_module_t;

typedef enum {
	CO_PRIORITY_DISCARDABLE=0,
	CO_PRIORITY_IMPORTANT,
} co_priority_t;

typedef enum {
	CO_MESSAGE_TYPE_STRING=0,
	CO_MESSAGE_TYPE_OTHER=1,
} co_message_type_t;

typedef struct {
	co_module_t from;
	co_module_t to;
	co_priority_t priority;
	co_message_type_t type;
	unsigned long size;
	unsigned char data[0];
} __attribute__((packed)) co_message_t;

typedef enum {
	CO_DEVICE_BLOCK=0,
	CO_DEVICE_CONSOLE,
	CO_DEVICE_KEYBOARD,
	CO_DEVICE_NETWORK,
	CO_DEVICE_TIMER,
	CO_DEVICE_POWER,
	CO_DEVICE_SERIAL,
	CO_DEVICE_FILESYSTEM,

	CO_DEVICE_MOUSE,
	CO_DEVICE_SCSI,
	CO_DEVICE_VIDEO,
	CO_DEVICE_AUDIO,
	CO_DEVICE_PCI,

	CO_DEVICES_TOTAL,
} co_device_t;

typedef enum {
	CO_KBD_SCANCODE_RAW=0,
	CO_KBD_SCANCODE_ASCII
} mode_data_from_keyboard_t;

typedef struct {
	unsigned char code;
	mode_data_from_keyboard_t mode;
} __attribute__((packed)) co_scan_code_t;

#define CO_MOUSE_MAX_X 2048
#define CO_MOUSE_MAX_Y 2048

typedef struct {
	unsigned	btns;
	unsigned	abs_x;
	unsigned	abs_y;
	int		rel_z;
} __attribute__((packed)) co_mouse_data_t;

typedef enum {
	CO_LINUX_MESSAGE_POWER_ALT_CTRL_DEL=0,
	CO_LINUX_MESSAGE_POWER_SHUTDOWN,
	CO_LINUX_MESSAGE_POWER_OFF,
} co_linux_message_power_type_t;

typedef struct {
	co_linux_message_power_type_t type;
} __attribute__((packed)) co_linux_message_power_t;

typedef struct {
	unsigned long tick_count;
} __attribute__((packed)) co_linux_message_idle_t;

typedef struct {
	co_device_t device;
	unsigned int unit;
	unsigned long size;
	char data[];
} __attribute__((packed)) co_linux_message_t;

typedef enum {
	CO_TERMINATE_END=0,
	CO_TERMINATE_REBOOT,
	CO_TERMINATE_POWEROFF,
	CO_TERMINATE_PANIC,
	CO_TERMINATE_HALT,
	CO_TERMINATE_FORCED_OFF,
	CO_TERMINATE_FORCED_END,
	CO_TERMINATE_INVALID_OPERATION,
	CO_TERMINATE_STACK_OVERFLOW,
	CO_TERMINATE_BUG,
	CO_TERMINATE_VMXE,
} co_termination_reason_t;

#ifdef CO_KERNEL

#ifndef asmlinkage
#define asmlinkage __attribute__((regparm(0)))
#endif

typedef void asmlinkage (*co_switcher_t)(co_arch_passage_page_t *page,
			      void *from,
			      void *to);

#define co_passage_page_func_low(_from_,_to_)	\
	(((co_switcher_t)(co_passage_page->code))	\
	 (co_passage_page,				\
	  &_from_.border2,			\
	  &_to_.border2))

# ifdef CO_COLINUX_KERNEL
#  define co_passage_page  ((co_arch_passage_page_t *)(CO_VPTR_PASSAGE_PAGE))
#  define co_current (co_passage_page->linuxvm_state)
#  define co_other (co_passage_page->host_state)
# else
#  define co_passage_page  (cmon->passage_page)
#  define co_other (co_passage_page->linuxvm_state)
#  define co_current (co_passage_page->host_state)
# endif

# define co_switch() co_passage_page_func_low(co_current, co_other)

#endif /* CO_KERNEL */

/*
 * Defines operations on various virtual devices.
 */

typedef enum {
	CO_OPERATION_CONSOLE_STARTUP=0,
	CO_OPERATION_CONSOLE_INIT=1,
	CO_OPERATION_CONSOLE_DEINIT,
	CO_OPERATION_CONSOLE_CLEAR,
	CO_OPERATION_CONSOLE_PUTC,
	CO_OPERATION_CONSOLE_PUTCS,
	CO_OPERATION_CONSOLE_CURSOR_DRAW,
	CO_OPERATION_CONSOLE_CURSOR_ERASE,
	CO_OPERATION_CONSOLE_CURSOR_MOVE,
	CO_OPERATION_CONSOLE_SCROLL_UP,
	CO_OPERATION_CONSOLE_SCROLL_DOWN,
	CO_OPERATION_CONSOLE_BMOVE,
	CO_OPERATION_CONSOLE_SWITCH,
	CO_OPERATION_CONSOLE_BLANK,
	CO_OPERATION_CONSOLE_FONT_OP,
	CO_OPERATION_CONSOLE_SET_PALETTE,
	CO_OPERATION_CONSOLE_SCROLLDELTA,
	CO_OPERATION_CONSOLE_SET_ORIGIN,
	CO_OPERATION_CONSOLE_SAVE_SCREEN,
	CO_OPERATION_CONSOLE_INVERT_REGION,
	CO_OPERATION_CONSOLE_CONFIG,
	CO_OPERATION_CONSOLE_INIT_SCROLLBUFFER,
} co_operation_console_t;


typedef char co_console_code;
typedef unsigned short co_console_character;
typedef unsigned short co_console_unit;

typedef struct {
	co_console_unit x;
	co_console_unit y;
	co_console_unit height;
} __attribute__((packed)) co_cursor_pos_t;

typedef struct {
	co_operation_console_t type;
	union {
		struct {
			co_console_unit top;
			co_console_unit bottom;
			co_console_unit lines;
			co_console_character charattr;
		} scroll;
		struct {
			co_console_unit y;
			co_console_unit x;
			co_console_unit count;
			co_console_character data[];
		} putcs;
		struct {
			co_console_unit x;
			co_console_unit y;
			co_console_character charattr;
		} putc;
		struct {
			co_console_unit top;
			co_console_unit left;
			co_console_unit bottom;
			co_console_unit right;
			co_console_character charattr;
		} clear;
		struct {
			co_console_unit y;
			co_console_unit x;
			co_console_unit count;
		} invert;
		struct {
			co_console_unit row;
			co_console_unit column;
			co_console_unit top;
			co_console_unit left;
			co_console_unit bottom;
			co_console_unit right;
		} bmove;
		struct {
			co_console_unit rows;
			co_console_unit cols;
			co_console_unit attr;
		} config;
		co_cursor_pos_t cursor;
	};
} __attribute__((packed)) co_console_message_t;

typedef struct {
	unsigned long messages_waiting;
	unsigned char buffer[];
} co_io_buffer_t;

typedef struct {
	unsigned long index;
	unsigned long flags;
	unsigned long func;
	unsigned long pid;
} __attribute__((packed)) co_trace_point_info_t;

typedef enum {
	CO_BLOCK_OPEN=0,
	CO_BLOCK_STAT,
	CO_BLOCK_READ,
	CO_BLOCK_WRITE,
	CO_BLOCK_CLOSE,
	CO_BLOCK_GET_ALIAS,
} co_block_request_type_t;

typedef enum {
	CO_BLOCK_REQUEST_RETCODE_OK=0,
	CO_BLOCK_REQUEST_RETCODE_ERROR=-1,
} co_block_request_retcode_t;

typedef enum {
	CO_NETWORK_GET_MAC=0,
} co_network_request_type_t;

#ifdef CO_KERNEL
/* If we are compiling kernel code (Linux or Host Driver) */
# ifdef CO_COLINUX_KERNEL
/* Inside Linux, vm_ptr_t considered a valid pointer in its virtual address space */
typedef void *vm_ptr_t;
#  else
/* But inside the host, the type is considered not to be a pointer in its own address space */
typedef unsigned long vm_ptr_t;
# endif

typedef struct {
	co_block_request_type_t type;
	long rc;
	union {
		struct {
			unsigned long long offset;
			unsigned long long size;
			unsigned long long disk_size;
			vm_ptr_t address;
			void * irq_request;
			int async;
		};
		struct {
			char alias[20];
		};
	};
} __attribute__((packed)) co_block_request_t;

typedef struct {
	void * irq_request;
	int uptodate;
} __attribute__((packed)) co_block_intr_t;

typedef struct {
	co_network_request_type_t type;
	unsigned int unit;
	char mac_address[6];
	char _pad[2];
	int result;
} __attribute__((packed)) co_network_request_t;

#endif /* CO_KERNEL */

typedef struct {
	unsigned long api_version;
	unsigned long compiler_major;
	unsigned long compiler_minor;
	unsigned long compiler_abi;
} __attribute__((packed)) co_info_t;

typedef struct {
	unsigned long co_core_end;
	unsigned long co_memory_size;
	void *co_initrd;
	unsigned long co_initrd_size;
	unsigned long co_cpu_khz;
	unsigned long filler[5];		// compatible old api: empty 5,6,7,8,9
	char co_boot_parameters[CO_BOOTPARAM_STRING_LENGTH]; // params[10]
} __attribute__((packed)) co_boot_params_t;

#ifndef COLINUX_TRACE
#define CO_TRACE_STOP
#define CO_TRACE_CONTINUE
#endif

#pragma pack()

#endif
