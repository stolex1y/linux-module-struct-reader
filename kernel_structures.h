#ifndef KERNEL_STRUCTURES
#define KERNEL_STRUCTURES

enum KERNEL_STRUCTS {
	MEMBLOCK_REG = 0,
	THGR_CPUTIMER = 1
};

struct my_thread_group_cputimer {
	int i;
	char b;
};

struct my_memblock_region {
	int i;
	char b;
};

static unsigned long const struct_sizes[] = {
	[MEMBLOCK_REG] = sizeof(struct my_memblock_region),
	[THGR_CPUTIMER] = sizeof(struct my_thread_group_cputimer)
};


#endif