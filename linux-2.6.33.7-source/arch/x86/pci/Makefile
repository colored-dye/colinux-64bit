obj-y				:= i386.o init.o

obj-$(CONFIG_PCI_COOPERATIVE)	+= copci.o irq_cooperative.o
obj-$(CONFIG_PCI_BIOS)		+= pcbios.o
obj-$(CONFIG_PCI_MMCONFIG)	+= mmconfig_$(BITS).o direct.o mmconfig-shared.o
obj-$(CONFIG_PCI_DIRECT)	+= direct.o
obj-$(CONFIG_PCI_OLPC)		+= olpc.o

obj-y				+= fixup.o
obj-$(CONFIG_ACPI)		+= acpi.o
obj-y				+= legacy.o
ifndef CONFIG_COOPERATIVE
obj-y				+= irq.o
endif

obj-$(CONFIG_X86_VISWS)		+= visws.o

obj-$(CONFIG_X86_NUMAQ)		+= numaq_32.o

obj-y				+= common.o
ifndef CONFIG_COOPERATIVE
obj-y				+= early.o
endif
obj-y				+= amd_bus.o
obj-$(CONFIG_X86_64)		+= bus_numa.o

ifeq ($(CONFIG_PCI_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif
