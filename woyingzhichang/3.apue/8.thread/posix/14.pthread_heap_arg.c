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
	int i;
	for (i = 0; i < 10; i++) {
		(*ptr)++;
		(*ptr)++;
		printf("pthread  p:%d\n", *ptr);
		sleep(1);
	}
}

int main(void){
	int * p;
	int i;
	pthread_t tid;
	p = malloc(sizeof(int));
	*p=111;
	ptr=p;
	pthread_create(&tid, NULL, func, NULL);

	for (i = 0; i < 10; i++) {
		(*p)--;
		printf("main p:%d\n", *p);
		sleep(2);
	}

	pthread_join(tid, NULL);
	exit(0);
}
