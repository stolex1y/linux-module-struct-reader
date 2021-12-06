//Программа на уровне пользователя получает на вход аргумент(ы) командной строки (не адрес!), позволяющие идентифицировать из системных таблиц необходимый путь до целевой структуры, осуществляет передачу на уровень ядра, получает информацию из данной структуры и распечатывает структуру в стандартный вывод.

#include "kernel_reader_ioctl_cmd.h"
#include "kernel_structures.h"


#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

static struct my_thread_group_cputimer* create_thgr_cputimer() {
	struct my_thread_group_cputimer* cputimer = malloc(struct_sizes[THGR_CPUTIMER]);
	return cputimer;
}

int main(void) {
	int fd = open("/dev/" DEVICE_FILE_NAME, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Не удалось открыть файл %s: ", DEVICE_FILE_NAME);
		perror("");
		return errno;
	}
	
	printf("Отправляю имя структуры\n");
	if (ioctl(fd, IOCTL_SET_STRUCT_NAME, THGR_CPUTIMER) == -1) {
		perror("Ошибка при запросе IOCTL_SET_STRUCT_NAME");
	} else {
		printf("Запрос к драйверу выполнен успешно\n");
	}
	
	printf("Запрашиваю выбранную структуру\n");
	struct my_thread_group_cputimer* const cputimer = create_thgr_cputimer();
	if (ioctl(fd, IOCTL_GET_STRUCT, cputimer) == -1) {
		perror("Ошибка при запросе IOCTL_GET_STRUCT");
	} else {
		printf("Получил cputimer: i = %d, b = %c\n", cputimer -> i, cputimer -> b);
	};
	
	close(fd);
	
}