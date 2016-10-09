#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT  30000000
#define RIGHT 30000200

#define THRNUM 4
static int num = 0;//任务池
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_num = PTHREAD_COND_INITIALIZER;
struct arg{
	int i;
};

void cal(int i, void* p);
static void* func(void *p);

int main(int argc, const char *argv[])
{
	int err, i;
	pthread_t tid[THRNUM];
	for (i = 0; i < THRNUM; i++) {
		err = pthread_create(tid+i, NULL, func, (void*)i);
		if (err) {
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}
	//为了比较pthread_cond 和 sched_yield 增加sleep 才能看到忙等中的大量盲目查询
	for (i = LEFT; i <= RIGHT; i++) {
		pthread_mutex_lock(&mut_num);
		while (num != 0) {
			pthread_cond_wait(&cond_num, &mut_num);
		}
		num = i;
		pthread_cond_signal(&cond_num);
		pthread_mutex_unlock(&mut_num);
		sleep(1);
	}

	//保证最后一个任务被消费掉
	pthread_mutex_lock(&mut_num);
	while (num != 0) {
		pthread_cond_wait(&cond_num, &mut_num);
	}
	num=-1;
	pthread_cond_broadcast(&cond_num);
	pthread_mutex_unlock(&mut_num);

	for (i = 0; i < THRNUM; i++) {
		pthread_join(tid[i], NULL);
	}
	pthread_mutex_destroy(&mut_num);
	pthread_cond_destroy(&cond_num);
	exit(0);
}

static void* func(void *p)
{
	int i;
	while (1) {
		pthread_mutex_lock(&mut_num);
		while (num == 0) {
			pthread_cond_wait(&cond_num, &mut_num);
		}
		if (num == -1) {
			pthread_mutex_unlock(&mut_num);
			break;
		}
		//printf("[%d]%d  get a number.\n",(int)p, num);
		i = num;
		num = 0;
		pthread_cond_broadcast(&cond_num);
		pthread_mutex_unlock(&mut_num);
		cal(i, p);
	}
	pthread_exit(NULL);
}

void cal(int i, void* p)
{
	int j,mark;
	mark = 1;
	for(j = 2; j < i/2; j++)
	{
		if(i % j == 0)
		{
			mark = 0;
			break;
		}
	}
	if(mark)
		printf("[%d]%d is a primer.\n",(int)p,i);
}


