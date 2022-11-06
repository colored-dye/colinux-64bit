/*
 *  linux/include/asm/cooperative.h
 *
 *  Copyright (C) 2004 Dan Aloni
 *
 *  This file defines the lower level interfaces between the Cooperative Linux
 *  kernel and the host OS driver. It's for both external inclusion from the
 *  and internal inclusion in the kernel sources.
 */

#ifndef __LINUX_ASM_COOPERATIVE_H__
#define __LINUX_ASM_COOPERATIVE_H__

typedef struct {
	unsigned short size;
	struct x86_idt_entry *table;
} __attribute__((packed)) x86_idt_t;

typedef struct {
	unsigned short limit;
	struct x86_dt_entry *base;
} __attribute__((packed)) x86_gdt_t;

typedef struct {
	unsigned char border2[0x4];

	unsigned long cs;
        #define CO_ARCH_STATE_STACK_CS "0x04"

	unsigned long ds;
        #define CO_ARCH_STATE_STACK_DS "0x08"

	unsigned long es;
        #define CO_ARCH_STATE_STACK_ES "0x0C"

	unsigned long cr3;
        #define CO_ARCH_STATE_STACK_CR3 "0x10"

	unsigned long cr4;
        #define CO_ARCH_STATE_STACK_CR4 "0x14"

	unsigned long cr2;
        #define CO_ARCH_STATE_STACK_CR2 "0x18"

	unsigned long cr0;
        #define CO_ARCH_STATE_STACK_CR0 "0x1C"

	x86_gdt_t gdt;
        #define CO_ARCH_STATE_STACK_GDT "0x20"

	unsigned long fs;
        #define CO_ARCH_STATE_STACK_FS  "0x26"

	unsigned long gs;
        #define CO_ARCH_STATE_STACK_GS  "0x2A"

	unsigned short ldt;
        #define CO_ARCH_STATE_STACK_LDT "0x2E"

	x86_idt_t idt;
        #define CO_ARCH_STATE_STACK_IDT "0x30"

	unsigned short tr;
        #define CO_ARCH_STATE_STACK_TR  "0x36"

	unsigned long return_eip;
        #define CO_ARCH_STATE_STACK_RETURN_EIP  "0x38"

	unsigned long flags;
        #define CO_ARCH_STATE_STACK_FLAGS "0x3C"

	unsigned long esp;
        #define CO_ARCH_STATE_STACK_ESP "0x40"

	unsigned long ss;
        #define CO_ARCH_STATE_STACK_SS "0x44"

	unsigned long dr0;
        #define CO_ARCH_STATE_STACK_DR0 "0x48"

	unsigned long dr1;
        #define CO_ARCH_STATE_STACK_DR1 "0x4C"

	unsigned long dr2;
        #define CO_ARCH_STATE_STACK_DR2 "0x50"

	unsigned long dr3;
        #define CO_ARCH_STATE_STACK_DR3 "0x54"

	unsigned long dr6;
        #define CO_ARCH_STATE_STACK_DR6 "0x58"

	unsigned long dr7;
        #define CO_ARCH_STATE_STACK_DR7 "0x5C"

	union {
		unsigned long temp_cr3;
		unsigned long other_map;
	} __attribute__((packed));
        #define CO_ARCH_STATE_STACK_TEMP_CR3 "0x60"
        #define CO_ARCH_STATE_STACK_OTHERMAP "0x60"

	unsigned long relocate_eip;
        #define CO_ARCH_STATE_STACK_RELOCATE_EIP "0x64"

	unsigned long pad1;
        #define CO_ARCH_STATE_STACK_RELOCATE_EIP_AFTER "0x68"

	unsigned long va;
        #define CO_ARCH_STATE_STACK_VA "0x6C"

	unsigned long sysenter_cs;
        #define CO_ARCH_STATE_SYSENTER_CS "0x70"

	unsigned long sysenter_esp;
        #define CO_ARCH_STATE_SYSENTER_ESP "0x74"

	unsigned long sysenter_eip;
        #define CO_ARCH_STATE_SYSENTER_EIP "0x78"
} __attribute__((packed)) co_arch_state_stack_t;

#define CO_MAX_PARAM_SIZE 0x400

typedef struct co_arch_passage_page_normal_address_space {
	unsigned long pgd[0x400];
	unsigned long pte[2][0x400];
} co_arch_passage_page_normal_address_space_t;

typedef struct co_arch_passage_page_pae_address_space {
	unsigned long long main[0x200];
	unsigned long long pgd[2][0x200];
	unsigned long long pte[2][0x200];
} co_arch_passage_page_pae_address_space_t;

typedef struct co_arch_passage_page {
	union {
		struct {
			union {
				struct {
					union {
						unsigned long self_physical_address;
						unsigned long temp_pgd_physical;
					} __attribute__((packed));
					unsigned long dr0;
					unsigned long dr1;
					unsigned long dr2;
					unsigned long dr3;
					unsigned long dr6;
					unsigned long dr7;
					unsigned char code[0x260];
				} __attribute__((packed));
				unsigned char pad[0x280]; /* Be careful! see NOTE below */
			} __attribute__((packed));

			/* Machine states */

			/*
			 * NOTE: *_state fields must be aligned at 16 bytes boundary since
			 * the fxsave/fxload instructions expect an aligned arugment.
			 */

			co_arch_state_stack_t host_state;
			co_arch_state_stack_t linuxvm_state;

			/* Control parameters */
			unsigned long operation;
			unsigned long params[];
		} __attribute__((packed));
		unsigned char first_page[0x1000];
	};

	/* page tables for passage address spaces */
	union {
		co_arch_passage_page_normal_address_space_t guest_normal;
		co_arch_passage_page_normal_address_space_t temp_space;
	} __attribute__((packed));
	union {
		co_arch_passage_page_normal_address_space_t host_normal;
		co_arch_passage_page_pae_address_space_t host_pae;
	} __attribute__((packed));
} co_arch_passage_page_t;

/*
 * Address space layout:
 */

#define CO_VPTR_BASE                         (0xffc00000UL)
#define CO_VPTR_PHYSICAL_TO_PSEUDO_PFN_MAP   (CO_VPTR_BASE - 0x1000000UL)
#define CO_VPTR_PSEUDO_RAM_PAGE_TABLES       (CO_VPTR_BASE - 0x1100000UL)
#define CO_VPTR_PASSAGE_PAGE                 (CO_VPTR_BASE - 0x1101000UL)
#define CO_VPTR_IO_AREA_SIZE                 (0x10000UL)
#define CO_VPTR_IO_AREA_START                (CO_VPTR_BASE - 0x1200000UL)
#define CO_VPTR_SELF_MAP                     (CO_VPTR_BASE - 0x1400000UL)

#define CO_VPTR_BASE_START			CO_VPTR_SELF_MAP
#define CO_VPTR_BASE_END			CO_VPTR_BASE

#define CO_LOWMEMORY_MAX_MB 984

typedef struct {
	unsigned long kernel_cs;
	unsigned long kernel_ds;
} __attribute__((packed)) co_arch_info_t;

#endif
