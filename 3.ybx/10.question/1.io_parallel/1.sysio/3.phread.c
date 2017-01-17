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

void test_sysio_read();
void test_sysio_write();

//多线程系统IO中:
//1.文件描述符数组是共享的
//2.内部结构体是独立的
int main(int argc, const char *argv[])
{
	test_sysio_read();
	//test_sysio_write();
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

/*
多线程中文件表项不共享，所以每个线程读取的都是文件头
./3.pthread
begin work tid:140329947027200
pthread_id:140329947027200, sleep(2)
begin work tid:140329926047488
pthread_id:140329926047488, sleep(2)
begin work tid:140329863108352
pthread_id:140329863108352, sleep(2)
begin work tid:140329884088064
pthread_id:140329884088064, sleep(2)
begin work tid:140329873598208
pthread_id:140329873598208, buf:# /etc/ser, len:10
begin work tid:140329905067776
pthread_id:140329905067776, buf:# /etc/ser, len:10
begin work tid:140329957517056
pthread_id:140329957517056, sleep(2)
begin work tid:140329894577920
pthread_id:188, buf:# /etc/ser, len:10
pthread_id:140329863108352, buf:# /etc/ser, len:10
pthread_id:140329884088064, buf:# /etc/ser, len:10
pthread_id:140329957517056, buf:# /etc/ser, len:10
pthread_id:140329894577920, buf:# /etc/ser, len:10
pthread_id:140329915557632, buf:# /etc/ser, len:10
*/
void test_sysio_read()
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

