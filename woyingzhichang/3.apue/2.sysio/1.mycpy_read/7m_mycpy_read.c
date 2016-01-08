#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE (1024*1024*7)

int main(int argc, const char *argv[])
{
	int count = 0;
	int num, ret, pos;
	int fd1, fd2;
	char str[SIZE];
	fd1 = open("1.pdf", O_RDONLY);
	if (fd1 == -1) {
		perror("open():");
		return -1;
	}
	fd2 = open("2.pdf", O_WRONLY|O_CREAT|O_TRUNC, 0660);
	if (fd2 == -1) {
		perror("open():");
		return -1;
	}

	while (1) {
		count ++;
		num = read(fd1, str, SIZE);
		if (num < 0) {
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
