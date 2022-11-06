
#ifndef __SCSI_COSCSI_H
#define __SCSI_COSCSI_H

#if defined(CO_KERNEL) || defined(CO_HOST_KERNEL)

/* Set this to 1 to enable background I/O in the host */
#define COSCSI_ASYNC 		1

typedef enum {
	CO_SCSI_GET_CONFIG,
	CO_SCSI_OPEN,
	CO_SCSI_CLOSE,
	CO_SCSI_SIZE,
	CO_SCSI_IO,
	CO_SCSI_PASS,
	CO_SCSI_IOS,
} CO_SCSI_OPS;

#define COSCSI_DEVICE_ENABLED 0x80

typedef struct {
	void *scp;
	unsigned long long offset;
	vm_ptr_t sg;
	int count;
	int reqlen;
	int write;
} __attribute__((packed)) co_scsi_io_t;

typedef struct {
	void *ctx;
	int result;
	int delta;
} __attribute__((packed)) co_scsi_intr_t;

typedef struct {
	unsigned char cdb[16];
	unsigned cdb_len: 7;
	unsigned write: 1;
	vm_ptr_t buffer;
	unsigned long buflen;
} __attribute__((packed)) co_scsi_pass_t;

#endif /* CO_KERNEL || CO_HOST_KERNEL */

/* Device types */
#define SCSI_PTYPE_DISK 		0x00
#define SCSI_PTYPE_TAPE			0x01
#define SCSI_PTYPE_PRINTER		0x02
#define SCSI_PTYPE_PROC			0x03
#define SCSI_PTYPE_WORM			0x04
#define SCSI_PTYPE_CDDVD		0x05
#define SCSI_PTYPE_SCANNER		0x06
#define SCSI_PTYPE_OPTICAL		0x07
#define SCSI_PTYPE_CHANGER		0x08
#define SCSI_PTYPE_COMM			0x09
#define SCSI_PTYPE_RAID			0x0C
#define SCSI_PTYPE_ENC			0x0D
#define SCSI_PTYPE_SDISK		0x0E
#define SCSI_PTYPE_CARD			0x0F
#define SCSI_PTYPE_BRIDGE		0x10
#define SCSI_PTYPE_OSD			0x11
#define SCSI_PTYPE_PASS			0x1F		/* Special */

#endif
