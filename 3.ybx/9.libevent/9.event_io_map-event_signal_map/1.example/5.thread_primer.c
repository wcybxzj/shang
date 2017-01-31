#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>   
#include <assert.h>

#include <event3/event.h>
#include <event3/util.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "util-internal.h"
#include <event3/thread.h>
#include "evthread-internal.h"

#define LEFT  30000000
#define RIGHT 30000200

#define THRNUM 4
static int num = 0;//任务池

void *mut_num;
void *cond_num;

struct arg{
	int i;
};

void cal(int i, void* p);
static void* func(void *p);

int main(int argc, const char *argv[])
{
	evthread_enable_lock_debuging();//对锁进行调试
	evthread_use_pthreads();

    EVTHREAD_ALLOC_LOCK(mut_num, 0);
    EVTHREAD_ALLOC_COND(cond_num);
	assert(mut_num);
	assert(cond_num);


	int err, i;
	pthread_t tid[THRNUM];
	for (i = 0; i < THRNUM; i++) {
		err = pthread_create(tid+i, NULL, func, (void*)i);
		if (err) {
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}
	for (i = LEFT; i <= RIGHT; i++) {
		EVLOCK_LOCK(mut_num, 0);

		while (num != 0) {
			assert(EVTHREAD_COND_WAIT(cond_num,mut_num) == 0);
		}
		num = i;
		EVTHREAD_COND_SIGNAL(cond_num);
		EVLOCK_UNLOCK(mut_num,0);
	}

	//保证最后一个任务被消费掉
	EVLOCK_LOCK(mut_num, 0);
	while (num != 0) {
		assert(EVTHREAD_COND_WAIT(cond_num,mut_num) == 0);
	}
	num=-1;
	EVTHREAD_COND_BROADCAST(cond_num);
	EVLOCK_UNLOCK(mut_num, 0);

	for (i = 0; i < THRNUM; i++) {
		pthread_join(tid[i], NULL);
	}
    EVTHREAD_FREE_LOCK(mut_num, 0);
    EVTHREAD_FREE_COND(cond_num);

	exit(0);
}

static void* func(void *p)
{
	int i;
	while (1) {
		EVLOCK_LOCK(mut_num, 0);
		while (num == 0) {
			assert(EVTHREAD_COND_WAIT(cond_num,mut_num) == 0);
		}
		if (num == -1) {
			EVLOCK_UNLOCK(mut_num,0);
			break;
		}
		//printf("[%d]%d  get a number.\n",(int)p, num);
		i = num;
		num = 0;
		EVTHREAD_COND_BROADCAST(cond_num);
		EVLOCK_UNLOCK(mut_num,0);
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
		printf("[%d][%d]%d is a primer.\n",(int)p,syscall(SYS_gettid),i);
}


