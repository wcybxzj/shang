#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM 4
pthread_mutex_t mut[NUM];

static int next(int n)
{
	if (n+1 == NUM) {
		return 0;
	}
	return n+1;
}

void* func(void *p)
{
	int i = (int)p;
	int c= 'a'+ i;
	while (1) {
		pthread_mutex_lock(mut+i);
		write(1, &c, 1);
		pthread_mutex_unlock(mut+next(i));
	}
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	pthread_t tid[NUM];
	int i, err;
	for (i = 0; i < NUM; i++) {
		pthread_mutex_init(mut+i, NULL);
		pthread_mutex_lock(mut+i);

		err = pthread_create(tid+i, NULL, func, (void *)i);
		if (err) {
			fprintf(stderr, "pthread_create():\n",strerror(err));
			exit(1);
		}
	}

	pthread_mutex_unlock(mut+0);
	alarm(5);

	//根本不执行
	for (i = 0; i < NUM; i++) {
		printf("join\n");
		pthread_join(tid[i], NULL);
	}

	exit(0);
}
