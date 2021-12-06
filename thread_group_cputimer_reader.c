#include "thread_group_cputimer_reader.h"
#include "thread_group_cputimer.h"
#include "kernel_structures.h"
#include "kernel_struct_reader.h"

#include <linux/slab.h>
#include <linux/module.h>

void* get_thread_group_cputimer(void) {
	struct my_thread_group_cputimer* cputimer = kmalloc(struct_sizes[THGR_CPUTIMER], GFP_KERNEL);
	cputimer -> i = 101;
	cputimer -> b = 'b';
	return cputimer;
}

MODULE_LICENSE("GPL");
