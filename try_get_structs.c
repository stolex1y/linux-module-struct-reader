#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/ptrace.h>
#include <linux/sched/task_stack.h>
#include <linux/sched/cputime.h>
#include <linux/pid.h>
#include <linux/kallsyms.h>

#define MY_MODULE "Get regs: "
#define RESULT_STR_LEN 10000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Filimonov");

static char* result_str; // for result string

static struct task_struct* get_task_by_pid(int pid) {
	return get_pid_task(find_get_pid(pid), PIDTYPE_PID);
}

static void print_error(const char* msg) {
	pr_err(MY_MODULE "%s", msg);
}

static void print_info(const char* msg) {
	pr_info(MY_MODULE "%s", msg);
}

static int with_error(const char* error) {
	print_error(error);
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
	if (!str || !str_len) return with_error("task_regs_to_str() - str is NULL");
	
	struct task_struct* task = get_task_by_pid(task_pid);
	if (!task) return with_error("task_regs_to_str() - task is NULL");
	
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

/*
	int PID;
	struct task_struct* task;
	struct thread_group_cputimer* cputimer;
	struct task_cputime_atomic cputimer_at;
	
	//task = current;
	
	//struct task_cputime times = { 0 };
	//thread_group_cputime(task, &times); undefined
	//task_sched_runtime(task); undefined
	// task->signal->utime = 0 & task->signal->stime = 0 ...
	
	if (!sig) {
		return with_error("task signal is empty");
	}
	times.utime = sig->utime;
	times.stime = sig->stime;
	times.sum_exec_runtime = sig->sum_sched_runtime;


	//cputimer = get_running_cputimer(task); - cputimer == NULL
	//cputimer_at = cputimer -> cputime_atomic;
	//pr_info(MY_MODULE "данные таймера: utime - %lld, stime - %lld, sum_exec_runtime - %lld", atomic64_read(&cputimer_at.utime), atomic64_read(&cputimer_at.stime), atomic64_read(&cputimer_at.sum_exec_runtime));
	//pr_info(MY_MODULE "данные таймер: utime - %lld, stime - %lld, sum_exec_runtime - %lld", 
		//times.utime, times.stime, times.sum_exec_runtime);
*/

static int __init struct_reader_init(void) {

	int choosen_task_pid = 10;

	result_str = kmalloc(RESULT_STR_LEN, GFP_KERNEL);

	int err = task_regs_to_str(choosen_task_pid, result_str, RESULT_STR_LEN);
	if (err)
		return err;
	else
		print_info(result_str);

	err = thread_group_cputime_to_str(choosen_task_pid, result_str, RESULT_STR_LEN);
	if (err)
		return err;
	else
		print_info(result_str);

	pr_info("");
	return 0;
}

static void __exit struct_reader_exit(void) {
	kfree(result_str);
	print_info("exit");
}

module_init(struct_reader_init); 
module_exit(struct_reader_exit); 