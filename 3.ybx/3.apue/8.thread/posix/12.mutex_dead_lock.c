#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

static int num = 1;
pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;
static void* func(void *p);


int main(int argc, const char *argv[])
{
	int err;
	int tid;
	err = pthread_create(&tid, NULL, func, NULL);
	sleep(1);//让子线程先上锁
	printf("parent thread block!!\n");
	err = pthread_mutex_lock(&mut_num);
	if (err) {
		fprintf(stderr, "pthread_mutex_lock %s\n",strerror(err));
	}
	printf("parent thread lock ok!!\n");
	return 0;
}

//死锁的原因是子线程没解锁
static void* func(void *p)
{
	int i, err;
	err = pthread_mutex_lock(&mut_num);
	if (err) {
		fprintf(stderr, "pthread_mutex_lock %s\n",strerror(err));
	}

	//解决办法:一旦子线程解锁,在父线程上锁的立即解除阻塞
	//for (i = 0; i < 3; i++) {
	//	sleep(1);
	//}
	//pthread_mutex_unlock(&mut_num);

	pthread_exit(NULL);
}

