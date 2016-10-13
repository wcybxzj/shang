#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define SIZE 10
#define FNAME "/etc/services"

#define THRNUM 10

void test_sysio_inner_struct();
void test_sysio_fd_arr();

//多线程系统IO中:
//1.文件描述符数组是共享的
//2.内部结构体是独立的
int main(int argc, const char *argv[])
{
	//test_sysio_inner_struct();
	test_sysio_fd_arr();
	return 0;
}

void* inner_struct(void *p)
{
	printf("begin work tid:%lu\n", pthread_self());
	int fd;
	char buf[SIZE];
	int len;
	memset(buf, 0x00, SIZE);
	//制作各线程的交叉执行效果
	if (pthread_self()%3) {
		printf("pthread_id:%lu, sleep(2)\n", pthread_self());
		sleep(2);
	}
	fd = open("/etc/services", O_RDONLY);
	if (fd<0) {
		perror("open():");
		exit(1);
	}
	len = read(fd, buf, SIZE);
	printf("pthread_id:%lu, buf:%s, len:%d\n", pthread_self(), buf, len);
	close(fd);
	pthread_exit(NULL);
}

void test_sysio_inner_struct()
{
	int i;
	pthread_t tid[THRNUM];
	for (i = 0; i < THRNUM; i++) {
		pthread_create(tid+i, NULL, inner_struct, NULL);
	}
	alarm(5);

	for (i = 0; i < THRNUM; i++) {
		pthread_join(tid[i], NULL);
	}
}

void *fd_arr(void *p){
	int fd;
	while (1) {
		fd = open(FNAME, O_RDONLY);
		if (fd < 0) {
			perror("open():");
			pthread_exit(0);
		}
		printf("child tid:%lu, fd:%d\n",pthread_self(), fd);
		sleep(1);
	}
	pthread_exit(0);
}

void test_sysio_fd_arr()
{
	int i;
	pthread_t tid[THRNUM];
	for (i = 0; i < THRNUM; i++) {
		pthread_create(tid+i, NULL, fd_arr, NULL);
	}
	alarm(5);
	for (i = 0; i < THRNUM; i++) {
		pthread_join(tid[i], NULL);
	}
}
