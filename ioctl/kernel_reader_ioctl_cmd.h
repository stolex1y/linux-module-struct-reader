#ifndef KERNEL_READER
#include <sys/ioctl.h>
#else
#include <linux/ioctl.h>
#endif

#define MAJOR_NUM 255

#define IOCTL_SET_STRUCT_NAME _IOW(MAJOR_NUM, 0, int)

#define IOCTL_GET_STRUCT _IOR(MAJOR_NUM, 1, void*) 

#define DEVICE_FILE_NAME "kernel_reader" 

enum KERNEL_STRUCTS {
	PT_REGS = 0,
	THGR_CPUTIMER = 1,
	NONE = -1
};

struct ioctl_query {
	enum KERNEL_STRUCTS kernel_struct_type;
	int pid;
};