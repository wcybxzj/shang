#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


void sys_err(const char *str)
{
	perror(str);
	exit(1);
}

int main(int argc, char *argv[])
{
	int num;
	int fd = open("1.txt",O_RDWR, 666);
	int fd1 = open("1.txt",O_RDWR, 666);
	if (fd==-1||fd1==-1) {
		sys_err("open()");
	}

	char *str1 = malloc(10);
	if (str1 == NULL) {
		sys_err("malloc()");
	}

	memset(str1, 0, 10);
	num = read(fd, str1, 1);
	printf("read() num:%d\n", num);
	printf("str1:%s\n", str1);//a


	memset(str1, 0, 10);
	num = read(fd1, str1, 1);
	printf("read() num:%d\n", num);
	printf("str1:%s\n", str1);//a

return 0;
}
