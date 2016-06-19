#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

#define NUM 4
static int num = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

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
		//李的代码很清晰
		//pthread_mutex_lock(&mut);
		//while(num != i){
		//	pthread_cond_wait(&cond, &mut);
		//}
		//write(1, &c, 1);
		//num = next(num);
		//pthread_cond_broadcast(&cond);
		//pthread_mutex_unlock(&mut);

		//如果是我的水平代码就会是这样
		pthread_mutex_lock(&mut);
		if (num==i) {
			write(1, &c, 1);
			num = next(num);
			pthread_cond_broadcast(&cond);
		}else {
			pthread_cond_wait(&cond, &mut);
		}
		pthread_mutex_unlock(&mut);

	}
	pthread_exit(NULL);
}


int main(int argc, const char *argv[])
{
	static pthread_t tid[NUM];
	int i, err;
	for (i = 0; i < NUM; i++) {
		err = pthread_create(tid+i, NULL, func, (void *)i);
		if (err) {
			fprintf(stderr, "pthread_create():\n",strerror(err));
			exit(1);
		}
	}

	alarm(5);
	//因为信号引起的异常终止,不会执行到 但是必须写
	for (i = 0; i < NUM; i++) {
		pthread_join(tid[i], NULL);
	}
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
	exit(0);
}
