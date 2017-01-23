#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#define SIZE 10
#define FNAME "/tmp/123.txt"
#define THRNUM 10

void test_stdio_read();
void test_stdio_write();

int main(int argc, const char *argv[])
{
	//test_stdio_read();
	test_stdio_write();
	return 0;
}

void test_stdio_read()
{
	//10.question/1.io_parallel/2.stdio/1.study_stdio/2.pthread.c
	//-->test_stdio_inner_struct()
}

void* my_write(void *p)
{
	printf("begin work tid:%lu\n", pthread_self());
	FILE *fp;
	char buf[SIZE];
	int i, len;
	//制作各线程的交叉执行效果
	if (pthread_self()%3) {
		printf("pthread_id:%lu, sleep(2)\n", pthread_self());
		sleep(2);
	}
	fp = fopen(FNAME, "a+");
	if (fp==NULL) {
		perror("fopen");
		exit(1);
	}
	for (i = 0; i < 100; i++) {
		//memset(buf, 0x00, SIZE);
		sprintf(buf, "tid:%d,i:%d\n", syscall(SYS_gettid), i); 
		fputs(buf, fp);
		//puts(buf);
	}

	fclose(fp);
	pthread_exit(NULL);
}

//10个线程每个100次写入正常
void test_stdio_write()
{
	int i;
	pthread_t tid[THRNUM];
	unlink(FNAME);
	for (i = 0; i < THRNUM; i++) {
		pthread_create(tid+i, NULL, my_write, NULL);
	}
	alarm(5);

	for (i = 0; i < THRNUM; i++) {
		pthread_join(tid[i], NULL);
	}
}
