#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define NUM 20
#define  FNAME "/tmp/out"
#define LEN 100

static void* func(void *p)
{
	FILE *fp;
	char str[LEN];
	int num;
	fp = fopen(FNAME, "r+");
	fgets(str, LEN, fp);
	num = atoi(str);
	num++;
	rewind(fp);
	sleep(1);//增加线程执行时间,放大问题代码的冲突
	fprintf(fp, "%d\n", num);
	fclose(fp);
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	int i;
	pthread_t tid[NUM];
	for (i = 0; i < 20; i++) {
		pthread_create(tid+i,NULL,func,NULL);
	}


	for (i = 0; i < 20; i++) {
		pthread_join(tid[i], NULL);
	}

	exit(0);
}


