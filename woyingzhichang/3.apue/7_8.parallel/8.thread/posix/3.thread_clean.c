#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void cleanup_func(void *p)
{
	puts(p);
}

static void * func(void *p)
{
	puts("thread is working!!!");

	pthread_cleanup_push(cleanup_func, "c11111");
	pthread_cleanup_push(cleanup_func, "c22222");
	pthread_cleanup_push(cleanup_func, "c33333");
	
	puts("push over");

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);

	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	int err;
	pthread_t tid;
	puts("Begin");

	err = pthread_create(&tid, NULL, func, NULL);
	if (err) {
		fprintf(stderr, "pthread_create:%s\n", strerror(err));
		exit(0);
	}

	pthread_join(tid, NULL);

	puts("End");
	return 0;
}
