#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT  30000000
#define RIGHT 30000200

#define THRNUM (RIGHT-LEFT+1)
int err;


struct arg{
	int i;
};

static void* func(void *p)
{
	struct arg *ptr= (struct arg *)p;
	int i = ptr->i;
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
	pthread_exit(ptr);
}

int main(int argc, const char *argv[])
{
	int i;
	void *ret;
	struct arg *ptr = NULL;
	pthread_t tid[THRNUM];

	for (i = LEFT; i <= RIGHT ; i++) {
		ptr = malloc(sizeof(struct arg));
		ptr->i = i;
		err = pthread_create(tid+(i-LEFT), NULL, func, ptr);
		if (err) {
			fprintf(stderr, "pthread_create(): %s\n", strerror(err));
			exit(1);
		}
	}

	for (i = LEFT; i <= RIGHT ; i++) {
		pthread_join(tid[i-LEFT], &ret);
		free(ret);
	}

	exit(0);
}
