#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "mysem.h"

#define LEFT  30000000
#define RIGHT 30000200
#define NUM 4
#define THRNUM (RIGHT-LEFT+1)
int err;

static void* func(void *p);

static mysem_t *me;
int main(int argc, const char *argv[])
{
	int i;
	pthread_t tid[THRNUM];

	me = mysem_init(NUM);

	for (i = LEFT; i <= RIGHT ; i++) {
		mysem_sub(me, 1);
		err = pthread_create(tid+(i-LEFT), NULL, func, (void*)i);
		if (err) {
			fprintf(stderr, "pthread_create(): %s\n", strerror(err));
			exit(1);
		}
	}

	for (i = LEFT; i <= RIGHT ; i++) {
		pthread_join(tid[i-LEFT], NULL);
	}

	exit(0);
}

static void* func(void *p)
{
	int i = (int)p;
	int j , mark;
	mark = 1;
	for (j = 2; j < i/2 ; j++) {
		if (i % j == 0) {
			mark = 0;
			break;
		}
	}
	//sleep(5);
	if (mark) {
		printf("%d is primer\n", i);
	}
	mysem_add(me,1);
}
