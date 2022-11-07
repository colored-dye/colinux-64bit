
#ifndef _LINUX_COPCI_H_
#define _LINUX_COPCI_H_

#if defined(CO_KERNEL) || defined(CO_HOST_KERNEL)

enum COPCI_DEVICE_REQUEST {
	COPCI_GET_CONFIG,
};

typedef struct {
	unsigned char dev;
	unsigned char func;
	unsigned char type;
	unsigned char unit;
} copci_config_t;

#endif

/* 32 devices, 8 funcs per device (fixed) */
#define COPCI_MAX_SLOTS 32
#define COPCI_MAX_FUNCS 8

enum COPCI_DEVICE_TYPE {
        COPCI_DT_NONE=0,
        COPCI_DT_VIDEO,
        COPCI_DT_AUDIO,
        COPCI_DT_IDE,
        COPCI_DT_SCSI,
        COPCI_DT_NET,
};

#endif
