/*
 *	Low-Level PCI Support for PC -- Routing of Interrupts
 *
 *	(c) 1999--2000 Martin Mares <mj@ucw.cz>
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <asm/pci_x86.h>

/*
 * Never use: 0, 1, 2 (timer, keyboard, and cascade)
 */
unsigned int pcibios_irq_mask = 0xfff8;

static int copci_enable_irq(struct pci_dev *pdev) {
	return 0;
}

static void copci_disable_irq(struct pci_dev *pdev) {
	return;
}

int (*pcibios_enable_irq)(struct pci_dev *dev) = copci_enable_irq;
void (*pcibios_disable_irq)(struct pci_dev *dev) = copci_disable_irq;
