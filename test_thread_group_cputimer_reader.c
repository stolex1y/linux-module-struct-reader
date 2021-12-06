#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/memblock.h>

#define MY_MODULE "Test 1: "
//static void* kernel_struct = NULL;

static int __init struct_reader_init(void) {
	#ifdef CONFIG_ARCH_KEEP_MEMBLOCK
		pr_info(MY_MODULE "config_arch установлен");
	#else
		pr_info(MY_MODULE "config_arch не установлен");
	#endif
	//int regions_count = 0;
	//regions_count = memblock.memory.cnt;
	//pr_alert(MY_MODULE "%d - regions_count, %p - memblock, %p", regions_count, &memblock, &memblock.memory);
	int regions_count = 100;
	struct memblock_region** regions = kmalloc(sizeof(void*) * regions_count, GFP_KERNEL);
	//struct memblock_region* regions = kmalloc(sizeof(void*) * regions_count, GFP_KERNEL);
	
	struct memblock_region* region = NULL;
	regions[0] = NULL;
	//int i = 0;
	//region = memblock.memory.regions;
	
	/*for_each_mem_region(region) {
		regions[i] = region;
		i++;
	}*/
	
	if (regions[0] == NULL) { 
        pr_alert(MY_MODULE "к сожалению, здесь ничего нет"); 
        return -1; 
    }

	
	pr_info(MY_MODULE "поздравляю! Всё получилось, вот, что я нашел: base = %lld, size = %lld, memblock_flags = %d", 
			regions[0] -> base, regions[0] -> size, regions[0] -> flags);
//	pr_info(MY_MODULE "%ld", memblock.memory.cnt);
	
	kfree(regions);
	//memblock_dump_all();
	return 0;
}

static void __exit struct_reader_exit(void) {
	
}

module_init(struct_reader_init); 
module_exit(struct_reader_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Filimonov");