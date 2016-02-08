#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define SIZE 9024000

//为2.sysio/mycpy.c 中的阻塞的系统调用open/read/write 增加假错EINTR处理
//但是我一直按着ctrl+c,没有一个open/read/write出现假错和2.star情况一样
void sig_handler(int s){
	printf("signal is comming\n");
}

int main(int argc, const char *argv[])
{
	int count = 0;
	int num, ret, pos;
	int fd1, fd2;
	char str[SIZE];

	signal(SIGINT, sig_handler);

	while ((fd1 = open("1.pdf", O_RDONLY)) < 0) {
		if (errno == EINTR) {
			printf("errno is EINTR EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
			continue;
		}
		perror("open():");
		return -1;
	}

	do {
		fd2 = open("2.pdf", O_WRONLY|O_CREAT|O_TRUNC, 0660);
		if (fd2 < 0) {
			if (errno == EINTR) {
				printf("errno is EINTR EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
				close(fd1);
				exit(1);
			}
		}
	} while (fd2 < 0);

	while (1) {
		count ++;
		printf("开始读!!!!!!!!!!!!!!!!!!\n");
		num = read(fd1, str, SIZE);
		if (num < 0) {
			if (errno == EINTR) {
				printf("errno is EINTR EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
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
			printf("开始写!!!!!!!!!!!!!!!!!!\n");
			ret = write(fd2, str+pos, num);
			if (ret < 0) {
				if (errno == EINTR) {
					printf("errno is EINTR EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
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
	close(fd2);
	return 0;
}
