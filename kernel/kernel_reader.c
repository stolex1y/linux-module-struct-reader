#define KERNEL_READER
#include "../ioctl/kernel_reader_ioctl_cmd.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h> 
#include <linux/cdev.h> 
#include <linux/device.h> 
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/ptrace.h>
#include <linux/sched/task_stack.h>
#include <linux/sched/cputime.h>
#include <linux/pid.h>
#include <linux/kallsyms.h>
#include <linux/string.h>

#define MY_MODULE "Kernel reader: "
#define KERNEL_STRUCT_STR_LEN 10000

//Разработать комплекс программ на пользовательском уровне и уровне ярда, который собирает информацию на стороне ядра и передает информацию на уровень пользователя, и выводит ее в удобном для чтения человеком виде.

//ioctl - передача параметров через управляющий вызов к файлу/устройству.
//Загружаемый модуль ядра принимает запрос через указанный в задании интерфейс, определяет путь до целевой структуры по переданному запросу и возвращает результат на уровень пользователя.

typedef int (get_struct)(const int task_pid, char* str, size_t str_len);

enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN = 1
};

// чтобы предотвратить множественный доступ к устройству
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED); 

static struct class *cls; 
static enum KERNEL_STRUCTS kernel_struct_type = NONE;
static int pid = 0;
static char* kernel_struct_str;


static struct task_struct* get_task_by_pid(int pid) {
	return get_pid_task(find_get_pid(pid), PIDTYPE_PID);
}

static int with_error(const char* error) {
	pr_err(MY_MODULE "%s", error);
	return -1;
}

static void get_regs_format_line(char* buf, const char* reg1, const char* reg2, const char* reg3, const char* reg4) {
	sprintf(buf, "%6s  %%20lu   |   %6s  %%20lu   |   %6s  %%20lu   |   %6s  %%20lu\n", reg1, reg2, reg3, reg4);
}


static int task_regs_to_str(const int task_pid, char* str, size_t str_len) {	
	if (!str || !str_len) return with_error("task_regs_to_str() - str is NULL");
	
	struct task_struct* task = get_task_by_pid(task_pid);
	if (!task) return with_error("task_regs_to_str() - task is NULL");
	
	const struct pt_regs* regs = task_pt_regs(task);
	if (!regs) return with_error("regs is NULL");
	
	*str = '\n';
	str++;
	
	char format_line[100];
	get_regs_format_line(format_line, "rax", "rbx", "rcx", "rdx");
	size_t char_count = snprintf(str, str_len, format_line, regs -> ax, regs -> bx, regs -> cx, regs -> dx);
	str_len -= char_count;
	str += char_count;
	
	get_regs_format_line(format_line, "rsi", "rdi", "rbp", "rsp");
	char_count = snprintf(str, str_len, format_line, regs -> si, regs -> di, regs -> bp, regs -> sp);
	str_len -= char_count;
	str += char_count;
	
	get_regs_format_line(format_line, "r8", "r9", "r10", "r11");
	char_count = snprintf(str, str_len, format_line, regs -> r8, regs -> r9, regs -> r10, regs -> r11);
	str_len -= char_count;
	str += char_count;
	
	get_regs_format_line(format_line, "r12", "r13", "r14", "r15");
	char_count = snprintf(str, str_len, format_line, regs -> r12, regs -> r13, regs -> r14, regs -> r15);
	str_len -= char_count;
	str += char_count;
	
	get_regs_format_line(format_line, "rip", "eflags", "", "");
	char_count = snprintf(str, str_len, format_line, regs -> ip, regs -> flags, 0, 0);
				
	return 0;
}

static int thread_group_cputime_to_str(const int task_pid, char* str, size_t str_len) {
	if (!str || !str_len) return with_error("thread_group_cputime_to_str() - str is NULL");
	
	struct task_struct* task = get_task_by_pid(task_pid);
	if (!task) return with_error("thread_group_cputime_to_str() - task is NULL");
	
	const struct pt_regs* regs = task_pt_regs(task);
	if (!regs) return with_error("regs is NULL");
	
	*str = '\n';
	str++;
	
	struct thread_group_cputimer cputimer = { 
		(struct task_cputime_atomic) {
			.utime = ATOMIC64_INIT(task_pid * 15600),
			.stime = ATOMIC64_INIT(task_pid * 4521),
			.sum_exec_runtime = ATOMIC64_INIT(task_pid * 22222),
		}
	};
	
	struct task_cputime_atomic cputimer_at = cputimer.cputime_atomic;
	struct task_cputime times = { 
		atomic64_read(&cputimer_at.utime), 
		atomic64_read(&cputimer_at.stime), 
		atomic64_read(&cputimer_at.sum_exec_runtime)
	};
	
	snprintf(
		str, str_len, 
		"%5s %15lld\n%5s %15lld\n%5s %15lld\n", 
		"user", times.utime, 
		"sys", times.stime, 
		"sum", times.sum_exec_runtime
	);
	
	return 0;
}

static const get_struct* const readers[2] = {
	[PT_REGS] = (get_struct*) task_regs_to_str,
	[THGR_CPUTIMER] = (get_struct*) thread_group_cputime_to_str
};

static int device_open(struct inode *inode, struct file *file) { 
    pr_info("device_open(%p,%p)\n", inode, file); 
 
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN) == CDEV_EXCLUSIVE_OPEN) 
        return -EBUSY; 
 
    try_module_get(THIS_MODULE); 
    return 0; 
}

static int device_release(struct inode *inode, struct file *file) 
{ 
    pr_info("device_release(%p,%p)\n", inode, file); 
 
    atomic_set(&already_open, CDEV_NOT_USED); 
 
    module_put(THIS_MODULE); 
    return 0; 
}  

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
	struct ioctl_query query = { .kernel_struct_type = NONE };
	switch (ioctl_num) {
		
		case IOCTL_SET_STRUCT_NAME:
			copy_from_user(&query, (void*) ioctl_param, sizeof(struct ioctl_query));
			kernel_struct_type = query.kernel_struct_type;
			pid = query.pid;
			if (kernel_struct_type == NONE) {
				return -EINVAL;
			}
			break;
			
		case IOCTL_GET_STRUCT:
			if (kernel_struct_type == NONE)
				return -EINVAL;
			int ret_val = readers[kernel_struct_type](pid, kernel_struct_str, (size_t) KERNEL_STRUCT_STR_LEN);
			if (ret_val != 0) return -EINVAL;
			return copy_to_user(
				(void*) ioctl_param, 
				kernel_struct_str, 
				strlen(kernel_struct_str)
			);
			
		default:
			return -ENOTTY;
	}
	return 0;
}


static struct file_operations fops = { 
    .unlocked_ioctl = device_ioctl, 
    .open = device_open, 
    .release = device_release 
}; 

static int __init struct_reader_init(void) {
	int ret_val = register_chrdev(MAJOR_NUM, DEVICE_FILE_NAME, &fops);
	if (ret_val < 0) { 
        pr_alert(MY_MODULE "error %d, couldn't register the dev %s\n", ret_val, DEVICE_FILE_NAME); 
        return ret_val; 
    } 
	
	cls = class_create(THIS_MODULE, DEVICE_FILE_NAME); 
	device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);
	pr_info(MY_MODULE "device created: /dev/%s\n", DEVICE_FILE_NAME); 
	
	kernel_struct_str = kmalloc(KERNEL_STRUCT_STR_LEN, GFP_KERNEL);
	
	return 0;
}

static void __exit struct_reader_exit(void) {
	device_destroy(cls, MKDEV(MAJOR_NUM, 0)); 
    class_destroy(cls);
	unregister_chrdev(MAJOR_NUM, DEVICE_FILE_NAME); 
	pr_info(MY_MODULE "the dev's deleted\n"); 
	
	kfree(kernel_struct_str);
}

module_init(struct_reader_init); 
module_exit(struct_reader_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Filimonov");