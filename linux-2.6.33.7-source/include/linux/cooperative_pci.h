
#ifndef __LINUX_COOPERATIVE_PCI_H
#define __LINUX_COOPERATIVE_PCI_H

/* PCI ids */
#define PCI_VENDOR_ID_CO        0x1a55
#define PCI_DEVICE_ID_COVIDEO   0x0001
#define PCI_DEVICE_ID_COIDE     0x0002
#define PCI_DEVICE_ID_COSCSI    0x0003
#define PCI_DEVICE_ID_COAUDIO   0x0004
#define PCI_DEVICE_ID_CONET     0x0005

/* NIC vendor registers */
#define PCI_CO_UNIT             0x40
#define PCI_CO_MAC1             0x41
#define PCI_CO_MAC2             0x42
#define PCI_CO_MAC3             0x43
#define PCI_CO_MAC4             0x44
#define PCI_CO_MAC5             0x45
#define PCI_CO_MAC6             0x46

#endif
