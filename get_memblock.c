#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/memblock.h>

#define MY_MODULE "Get memblock: "

extern struct memblock memblock;

size_t regions_count;
struct memblock_region** regions;
struct memblock_region* region;
size_t region_index;


static int __init struct_reader_init(void) {
	#ifdef CONFIG_ARCH_KEEP_MEMBLOCK
		pr_info(MY_MODULE "CONFIG_ARCH_KEEP_MEMBLOCK is set");
	#else
		pr_info(MY_MODULE "CONFIG_ARCH_KEEP_MEMBLOCK is not set");
	#endif


	regions_count = memblock.memory.cnt;
	//regions_count = 100;
	regions = kmalloc(sizeof(void*) * regions_count, GFP_KERNEL);
	
	region = NULL;
	regions[0] = NULL;
	region_index = 0;
	
	/*for_each_mem_region(region) {
		regions[i] = region;
		i++;
		pr_info(MY_MODULE "region[%d]: base = %lld, size = %lld, memblock_flags = %d", 	
			i, regions[0] -> base, regions[0] -> size, regions[0] -> flags);
	}*/
	
	if (regions[0] == NULL) { 
        pr_alert(MY_MODULE "memblock regions is null"); 
        return -1; 
    }
	
	return 0;
}

static void __exit struct_reader_exit(void) {
	kfree(regions);
}

module_init(struct_reader_init); 
module_exit(struct_reader_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Filimonov");