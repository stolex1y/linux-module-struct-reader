//Программа на уровне пользователя получает на вход аргумент(ы) командной строки (не адрес!), позволяющие идентифицировать из системных таблиц необходимый путь до целевой структуры, осуществляет передачу на уровень ядра, получает информацию из данной структуры и распечатывает структуру в стандартный вывод.

#include "../ioctl/kernel_reader_ioctl_cmd.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Need two arguments, e.g. 1 10, where 1 is kernel struct and 10 is pid\n");
		return 0;
	}
	
	int kernel_type = atoi(argv[1]);
	if (kernel_type > 1 || kernel_type < -1) kernel_type = NONE;
	int pid = atoi(argv[2]);
	
	int fd = open("/dev/" DEVICE_FILE_NAME, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Couldn't open the file %s: ", DEVICE_FILE_NAME);
		perror("");
		return errno;
	}
	
	struct ioctl_query query = {
		.kernel_struct_type = kernel_type,
		.pid = pid
	};
	
	if (ioctl(fd, IOCTL_SET_STRUCT_NAME, &query) == -1) {
		perror("Invalid struct number");
		return errno;
	}
	
	char result[10000];
	if (ioctl(fd, IOCTL_GET_STRUCT, result) == -1) {
		perror("Didn't find task with this pid");
	} else {
		printf("%s\n", result);
	};
	
	close(fd);
	
}