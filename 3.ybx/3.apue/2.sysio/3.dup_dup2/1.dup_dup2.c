#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME "/tmp/out"

void normal_test()
{
	int fd, back_fd;
	back_fd = dup(1);//backup stdout
	printf("backfd is :%d\n", back_fd);//back_fd is 3
	close(1);
	fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
	/**************************************/
	puts("hello normal");
	fflush(NULL);
	if (fd != 1) {
		close(fd);
	}
	close(1);
	dup(back_fd);
	puts("you will see me in terminal");
}

void dup_test()
{
	int fd, fd_back;
	fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
	fd_back = dup(1);//备份stdout
	close(1);
	dup(fd);
	if (fd!=1) {
		close(fd);
	}
	/**************************************/
	puts("hello dup");
	fflush(NULL);
	close(1);
	dup(fd_back);//回复stdout
	//dup2(fd_back, 1);//回复stdout另外一种写法
	puts("this line will dissply in terminal");
}

void dup2_test()
{
	int fd, back_fd;
	fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
	back_fd = dup(1);
	dup2(fd, 1);
	if (fd!=1) {
		close(fd);
	}
	/**************************************/
	puts("hello dup2");
	fflush(NULL);
	dup2(back_fd,1);
	puts("this line will dissply in terminal");
}


//使用不同方法,目的让hello写到文件中去
int main(int argc, const char *argv[])
{
	normal_test();
	//dup_test();
	//dup2_test();
	return 0;
}
