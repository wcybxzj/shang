#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

//select 3秒超时没事件发生返回0
int main(int argc, const char *argv[])
{
	int fd1, num;
	fd1 = open("/dev/tty11",O_CREAT|O_RDWR);
	if (fd1<0) {
		perror("open():");
		exit(1);
	}

	struct timeval tv;
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd1, &set);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	if ( (num=select(fd1+1, &set, NULL, NULL, &tv))<0 ) {
		perror("select:");
		exit(2);
	}
	printf("num is %d\n", num);
	printf("select is ok\n");

	return 0;
}
