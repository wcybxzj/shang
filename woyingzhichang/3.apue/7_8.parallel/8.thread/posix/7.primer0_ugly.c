#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT  30000000
#define RIGHT 30000200

#define THRNUM (RIGHT-LEFT+1)
int err;

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

	if (mark) {
		printf("%d is primer\n", i);
	}
}


int main(int argc, const char *argv[])
{
	int i;
	pthread_t tid[THRNUM];

	for (i = LEFT; i <= RIGHT ; i++) {
		//printf("main i:%d\n", i);
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
