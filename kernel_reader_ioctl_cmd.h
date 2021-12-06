#ifndef KERNEL_STRUCT_READER_H
#include <sys/ioctl.h>
#else
#include <linux/ioctl.h>
#endif

#define MAJOR_NUM 255

#define IOCTL_SET_STRUCT_NAME _IOW(MAJOR_NUM, 0, int)

#define IOCTL_GET_STRUCT _IOR(MAJOR_NUM, 1, void*) 

#define DEVICE_FILE_NAME "kernel_reader" 