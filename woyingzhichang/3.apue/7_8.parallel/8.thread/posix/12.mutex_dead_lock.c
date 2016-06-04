#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int num = 1;
pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;
static void* func(void *p);

int main(int argc, const char *argv[])
{
	int err;
	int tid;
	err = pthread_create(&tid, NULL, func, NULL);
	sleep(1);/////////////////////////////////////////////////////
	printf("will block forever!!\n");
	err = pthread_mutex_lock(&mut_num);
	if (err) {
		fprintf(stderr, "pthread_mutex_lock %s\n",strerror(err));
	}
	printf("never see it!\n");
	pthread_join(&tid, NULL);
	return 0;
}

static void* func(void *p)
{
	int err;
	err = pthread_mutex_lock(&mut_num);
	if (err) {
		fprintf(stderr, "pthread_mutex_lock %s\n",strerror(err));
	}
	pthread_exit(NULL);
}

