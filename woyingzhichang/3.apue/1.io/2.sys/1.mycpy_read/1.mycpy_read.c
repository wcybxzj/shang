#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 1024

int main(int argc, const char *argv[])
{
	int num, ret, pos;
	int fd1, fd2;
	char str[SIZE];
	fd1 = open("1.txt", O_RDONLY);
	if (fd1 == -1) {
		perror("open():");
		return -1;
	}
	fd2 = open("2.txt", O_WRONLY|O_CREAT|O_TRUNC, 0660);
	if (fd2 == -1) {
		perror("open():");
		return -1;
	}

	while (1) {
		num = read(fd1, str, SIZE);
		if (num < 0) {
			perror("read():");
			break;
		}
		if (num == 0) {
			break;
		}
		//要写num个字符,如果没写到就多写几次
		//更严谨但是这里看不到什么特殊效果
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
	close(fd1);
	close(fd2);
	return 0;
}
