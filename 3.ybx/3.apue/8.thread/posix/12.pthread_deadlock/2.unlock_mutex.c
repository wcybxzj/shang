#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

static int num = 1;
pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;
static void* func(void *p);


//解锁一个没有上锁的mutex 不会报错
int main(int argc, const char *argv[])
{
	int err;
	int tid;

	err = pthread_mutex_unlock(&mut_num);
	printf("%d", err);

	if (err) {
		fprintf(stderr, "pthread_mutex_lock %s\n",strerror(err));
	}

	return 0;
}