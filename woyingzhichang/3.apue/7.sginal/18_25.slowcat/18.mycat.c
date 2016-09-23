#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

//无流控 普通mycat
#define SIZE 1024

int main(int argc, char *argv[])
{
	int count = 0;
	int num, ret, pos;
	int fd1, fd2=1;
	char str[SIZE];

	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	while ((fd1 = open(argv[1], O_RDONLY)) < 0) {
		if (errno == EINTR) {
			continue;
		}
		perror("open():");
		return -1;
	}

	while (1) {
		count ++;
		num = read(fd1, str, SIZE);
		if (num < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("read():");
			break;
		}
		if (num == 0) {
			break;
		}
		//坚持写够num个字节，因为信号会打断阻塞的系统调用
		pos = 0;
		while (num > 0) {
			ret = write(fd2, str+pos, num);
			if (ret < 0) {
				if (errno == EINTR) {
					continue;
				}
				perror("write():");
				return -2;
			}
			num -= ret;
			pos += ret;
		}

	}
	printf("循环次数 %d\n", count);
	close(fd1);
	return 0;
}
