#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/memblock.h>
#include <linux/sched/cputime.h>
#include <linux/pid.h>
#include <linux/kallsyms.h>

//#include <linux/atomic/atomic-instrumented.h>

#define MY_MODULE "Test 1: "
//static void* kernel_struct = NULL;

extern struct memblock memblock;

int PID;
struct task_struct* task;
struct thread_group_cputimer* cputimer;
struct task_cputime_atomic cputimer_at;
struct task_struct *g, *p;


static int __init struct_reader_init(void) {
	#ifdef CONFIG_ARCH_KEEP_MEMBLOCK
		pr_info(MY_MODULE "config_arch установлен");
	#else
		pr_info(MY_MODULE "config_arch не установлен");
	#endif
	#ifdef CONFIG_POSIX_TIMERS
		pr_info(MY_MODULE "posix set");
	#else
		pr_info(MY_MODULE "posix doesn't set");
	#endif
	
	PID = 1;
	task = get_pid_task(find_get_pid(PID), PIDTYPE_PID);
	if (!task) {
		pr_alert(MY_MODULE "такого процесса не существует");
		return -1;
	}
	else
		pr_info(MY_MODULE "состояние процесса с PID = %d - %ld", PID, task->state);

	//struct task_cputime task_cputime;
	//task_cputime = (struct task_cputime) { 100, 100, 100 };
	//((f*)func)(task, &task_cputime);
	//struct task_cputime task_cputime;
	//task_cputime = (struct task_cputime) { 100, 100, 100 };
	//thread_group_cputime(task, &task_cputime);
	//cputimer = get_running_cputimer(task);

//	cputimer = &task->signal->cputimer;
	/*if (cputimer) {
		cputimer_at = cputimer -> cputime_atomic;
		pr_info(MY_MODULE "данные таймера: utime - %lld, stime - %lld, sum_exec_runtime - %lld", atomic64_read(&cputimer_at.utime), atomic64_read(&cputimer_at.stime), atomic64_read(&cputimer_at.sum_exec_runtime));
	} else {
		pr_alert(MY_MODULE "пустой cputimer");
	}*/
	
	//int regions_count = 0;
	//regions_count = memblock.memory.cnt;
	//pr_alert(MY_MODULE "%d - regions_count, %p - memblock, %p", regions_count, &memblock, &memblock.memory);
	int regions_count = 100;
	struct memblock_region** regions = kmalloc(sizeof(void*) * regions_count, GFP_KERNEL);
	//struct memblock_region* regions = kmalloc(sizeof(void*) * regions_count, GFP_KERNEL);
	
	struct memblock_region* region = NULL;
	regions[0] = NULL;
	int i = 0;
	//region = memblock.memory.regions;
	
	for_each_mem_region(region) {
		regions[i] = region;
		i++;
		//pr_info(MY_MODULE "поздравляю! Всё получилось, вот, что я нашел: base = %lld, size = %lld, memblock_flags = %d", 
			//regions[0] -> base, regions[0] -> size, regions[0] -> flags);
	}
	pr_info(MY_MODULE "%d", i);
	
	/*if (regions[0] == NULL) { 
        pr_alert(MY_MODULE "к сожалению, здесь ничего нет"); 
        return -1; 
    }*/

	
	//pr_info(MY_MODULE "поздравляю! Всё получилось, вот, что я нашел: base = %lld, size = %lld, memblock_flags = %d", 
		//	regions[0] -> base, regions[0] -> size, regions[0] -> flags);
//	pr_info(MY_MODULE "%ld", memblock.memory.cnt);
	
	//kfree(regions);
	//memblock_dump_all();
	return 0;
}

static void __exit struct_reader_exit(void) {
	
}

module_init(struct_reader_init); 
module_exit(struct_reader_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Filimonov");