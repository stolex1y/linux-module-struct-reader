//Разработать комплекс программ на пользовательском уровне и уровне ярда, который собирает информацию на стороне ядра и передает информацию на уровень пользователя, и выводит ее в удобном для чтения человеком виде.
#include "kernel_struct_reader.h"
#include "kernel_structures.h"


#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h> 
#include <linux/cdev.h> 
#include <linux/device.h> 
#include <linux/types.h>

#define MY_MODULE "Kernel reader: "

//ioctl: memblock_region, thread_group_cputimer
//ioctl - передача параметров через управляющий вызов к файлу/устройству.
//Загружаемый модуль ядра принимает запрос через указанный в задании интерфейс, определяет путь до целевой структуры по переданному запросу и возвращает результат на уровень пользователя.
//Целевая структура может быть задана двумя способами:

//Именем структуры в заголовочных файлах Linux
//Файлом в каталоге /proc. В этом случае необходимо определить целевую структуру по пути файла в /proc и выводимым данным.

enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED); 

static struct class *cls; 
static int kernel_struct_type = -1;
static void* kernel_struct = NULL;

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

static long 
device_ioctl(struct file *file, 
             unsigned int ioctl_num,
             unsigned long ioctl_param) {
	switch (ioctl_num) {
		case IOCTL_SET_STRUCT_NAME:
			if ((int) ioctl_param < 0 && (int) ioctl_param >= struct_readers.size) {
				kernel_struct_type = -1;
				return -EINVAL;
			}
			kernel_struct_type = (int) ioctl_param;
			break;
		case IOCTL_GET_STRUCT:
			if (kernel_struct_type == -1)
				return -EINVAL;
			kernel_struct = struct_readers.readers[kernel_struct_type]();
			return copy_to_user((void*) ioctl_param, kernel_struct, struct_sizes[kernel_struct_type]);
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
        pr_alert(MY_MODULE "ошибка %d, зарегистрировать устройство не удалось\n", ret_val); 
        return ret_val; 
    } 
	
	cls = class_create(THIS_MODULE, DEVICE_FILE_NAME); 
	device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);
	
	pr_info(MY_MODULE "устройство создано: /dev/%s\n", DEVICE_FILE_NAME); 
	
	return 0;
}

static void __exit struct_reader_exit(void) {
	device_destroy(cls, MKDEV(MAJOR_NUM, 0)); 
    class_destroy(cls);
	unregister_chrdev(MAJOR_NUM, DEVICE_FILE_NAME); 
	
	pr_info(MY_MODULE "устройство удалено\n"); 
}

module_init(struct_reader_init); 
module_exit(struct_reader_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Filimonov");