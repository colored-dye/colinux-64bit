
#ifndef _LINUX_COVIDEO_H_
#define _LINUX_COVIDEO_H_

enum CO_VIDEO_FUNC {
	CO_VIDEO_GET_CONFIG,
	CO_VIDEO_TEST,
};

struct covideo_config {
	void *buffer;
	int size;
};

typedef struct covideo_config covideo_config_t;

#endif
