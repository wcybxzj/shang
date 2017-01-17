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

void test_stdio_inner_struct();
void test_stdio_fd_arr();

int main(int argc, const char *argv[])
{
	//test_stdio_inner_struct();
	test_stdio_fd_arr();
	return 0;
}

void* inner_struct(void *p)
{
	printf("begin work tid:%lu\n", pthread_self());
	FILE *fp;
	char buf[SIZE];
	int len;
	memset(buf, 0x00, SIZE);
	//制作各线程的交叉执行效果
	if (pthread_self()%3) {
		printf("pthread_id:%lu, sleep(2)\n", pthread_self());
		sleep(2);
	}
	fp = fopen(FNAME, "r");
	fgets(buf, SIZE, fp);
	printf("pthread_id:%lu, buf:%s, len:%d\n", pthread_self(), buf, len);
	fclose(fp);
	pthread_exit(NULL);
}

/*
多线程中读取的是一样的内容
./2.pthread 
begin work tid:140300718130944
begin work tid:140300707641088
pthread_id:140300707641088, sleep(2)
begin work tid:140300697151232
pthread_id:140300697151232, sleep(2)
pthread_id:140300718130944, buf:# /etc/se, len:32666
begin work tid:140300609648384
pthread_id:140300609648384, sleep(2)
begin work tid:140300599158528
begin work tid:140300588668672
pthread_id:140300588668672, sleep(2)
pthread_id:140300599158528, buf:# /etc/se, len:0
begin work tid:140300578178816
pthread_id:140300578178816, sleep(2)
begin work tid:140300567688960
begin work tid:140300557199104
pthread_id:140300557199104, sleep(2)
pthread_id:140300567688960, buf:# /etc/se, len:0
begin work tid:140300475430656
pthread_id:140300475430656, buf:# /etc/se, len:0
pthread_id:140300707641088, buf:# /etc/se, len:0
pthread_id:140300697151232, buf:# /etc/se, len:0
pthread_id:140300588668672, buf:# /etc/se, len:0
pthread_id:140300578178816, buf:# /etc/se, len:0
pthread_id:140300609648384, buf:# /etc/se, len:0
pthread_id:140300557199104, buf:# /etc/se, len:0
*/
void test_stdio_inner_struct()
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
	FILE *fp;
	while (1) {
		fp = fopen(FNAME, "r");
		printf("child tid:%lu, fp:%x\n",pthread_self(), fp);
		sleep(1);
	}
	pthread_exit(0);
}

/*什么也不说明
./2.pthread 
child tid:139802410710784, fp:400008c0
child tid:139802431690496, fp:280008c0
child tid:139802421200640, fp:300008c0
child tid:139802389731072, fp:2c0008c0
child tid:139802400220928, fp:380008c0
child tid:139802379241216, fp:200008c0
child tid:139802347771648, fp:240008c0
child tid:139802337281792, fp:180008c0
child tid:139802368751360, fp:1c0008c0
child tid:139802358261504, fp:3c0008c0
child tid:139802389731072, fp:2c000b00
child tid:139802400220928, fp:38000b00
child tid:139802431690496, fp:28000b00
child tid:139802410710784, fp:40000b00
child tid:139802421200640, fp:30000b00
child tid:139802379241216, fp:20000b00
child tid:139802337281792, fp:18000b00
child tid:139802347771648, fp:24000b00
child tid:139802358261504, fp:3c000b00
child tid:139802368751360, fp:1c000b00
*/
void test_stdio_fd_arr()
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
