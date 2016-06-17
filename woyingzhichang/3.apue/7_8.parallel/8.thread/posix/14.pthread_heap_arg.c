#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

static int *ptr;

static void* func(void *p){
	printf("pthread  p:%d\n", *ptr);
	*ptr=222;
}

int main(void){
	int * p;
	pthread_t tid;
	p = malloc(sizeof(int));
	*p=111;
	ptr=p;
	printf("before thread func p:%d\n", *p);
	pthread_create(&tid, NULL, func, NULL);
	pthread_join(tid, NULL);
	printf("after thread func p:%d\n", *p);
	exit(0);
}
