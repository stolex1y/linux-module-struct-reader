/*
 *	Header-файл с ioctl командами для модуля kernel_struct_reader,
 *	с помощью которого можно прочитать следующие структуры ядра:
 * 	memblock_region,
 *	thread_group_cputimer
 */

#ifndef KERNEL_STRUCT_READER_H
#define KERNEL_STRUCT_READER_H

#include "kernel_structures.h"
#include "thread_group_cputimer_reader.h"
#include "memblock_region_reader.h"
#include "kernel_reader_ioctl_cmd.h"

#include <linux/ioctl.h> 
#include <linux/types.h>

typedef void* (get_struct)(void);

static const struct {
	int size;
	get_struct* const readers[2];
} struct_readers = {
	.size = 2,
	.readers = {
		[MEMBLOCK_REG] = (get_struct*) get_memblock_region,
		[THGR_CPUTIMER] = (get_struct*) get_thread_group_cputimer
	}
};

#endif