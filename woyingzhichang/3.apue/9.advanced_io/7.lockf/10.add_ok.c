#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM 20
#define  FNAME "/tmp/out"
#define LEN 100


static void* func(void *p)
{
	FILE *fp;
	int fd;
	char str[LEN];
	int num;
	fp = fopen(FNAME, "r+");
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0);
	fgets(str, LEN, fp);
	num = atoi(str);
	num++;
	rewind(fp);
	sleep(1);
	fprintf(fp, "%d\n", num);
	lockf(fd, F_ULOCK, 0);
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


