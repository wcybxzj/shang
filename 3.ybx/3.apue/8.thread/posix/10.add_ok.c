#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#define NUM 20
#define  FNAME "/tmp/out"
#define LEN 100

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void* func(void *p)
{
	FILE *fp;
	char str[LEN];
	int num;
	fp = fopen(FNAME, "r+");
	if (!fp) {
		perror("fopen");
		exit(1);
	}
	
	pthread_mutex_lock(&mut);
	fgets(str, LEN, fp);
	num = atoi(str);
	num++;
	rewind(fp);
	sleep(1);
	fprintf(fp, "%d\n", num);
	fclose(fp);
	pthread_mutex_unlock(&mut);
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

	pthread_mutex_destroy(&mut);
	exit(0);
}


