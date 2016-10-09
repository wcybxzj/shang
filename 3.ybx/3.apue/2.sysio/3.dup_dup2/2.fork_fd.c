#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 10
#define FNAME "/etc/services"

void test_sysio_inner_struct();
void test_sysio_fd_arr();

//http://blog.csdn.net/ordeder/article/details/21716639
//fork父子在系统IO中:
//1.文件描述符数组是各自独立的
//2.内部结构体是共享的
int main(int argc, const char *argv[])
{
	test_sysio_inner_struct();
	//test_sysio_fd_arr();
	return 0;
}

void test_sysio_inner_struct()
{
	pid_t pid;
	int fd;
	char buf[SIZE];
	fd = open("/etc/services", O_RDONLY);
	if (fd<0) {
		perror("open():");
		exit(1);
	}

	pid =  fork();
	if (pid==0) {
		read(fd, buf, SIZE);
		printf("child pid:%d, buf:%s\n", getpid(), buf);
		close(fd);
		exit(0);
	}

	read(fd, buf, SIZE);
	printf("parent pid:%d, buf:%s\n", getpid(), buf);
	wait(NULL);
	close(fd);
}

void test_sysio_fd_arr()
{
	int fd, i=0;
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		while (1) {
			fd = open(FNAME, O_RDONLY);
			if (fd < 0) {
				perror("open():");
				exit(1);
			}
			printf("child pid:%d, fd:%d\n", getpid(), fd);
			i++;
			if (i%3==0) {
				sleep(1);
			}
		}
		exit(0);
	}
	while (1) {
		fd = open(FNAME, O_RDONLY);
		if (fd < 0) {
			perror("open():");
			exit(1);
		}
		printf("parent pid:%d, fd:%d\n", getpid(), fd);
		i++;
		if (i%3==0) {
			sleep(5);
		}
	}
	wait(NULL);
}
