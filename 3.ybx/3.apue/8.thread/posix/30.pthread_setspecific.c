#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

#define nthreads 2
static pthread_key_t item_lock_type_key;

struct st {
	int id;
};

struct st *threads=NULL;


pid_t gettid()
{
     return syscall(SYS_gettid);
}

void* func(void* arg)
{
	int i;
	struct st* me = arg;
	pthread_setspecific(item_lock_type_key, &me->id);
	me->id=22222222;
	for (i = 0; i <20; i++) {
		printf("tid:%d, id:%d\n",gettid(), me->id);
		sleep(1);
	}
}

//证明:
//1.如果线程私有数据如果是在main thread中申请的
//这个数据在其他线程一样可以修改
//2.线程私有数据常用方法是像tlpi/threads/strerror_tsd.c
//那样在子线程malloc,利用线程私有数据之前设置好的是析构函数来free
int main(int argc, const char *argv[])
{
	int i;
	int ret;
	pthread_t tid;

	threads = calloc(nthreads, sizeof(struct st));
	for (i = 0; i < nthreads; i++) {
		threads[i].id = 11111111;
	}

	for (i = 0; i < nthreads; i++) {
		printf("main thread, id:%d\n", threads[i].id);
	}

	ret = pthread_key_create(&item_lock_type_key, NULL);
	if (ret) {
		perror("pthread_key_create()");
		exit(1);
	}

	for (i = 0; i < nthreads; i++) {
		pthread_create(&tid, NULL, func, &threads[i]);
	}

	sleep(1);//保证子线程都创建完

	for (i = 0; i < nthreads; i++) {
		printf("main thread, id:%d\n", threads[i].id);
	}

	for (i = 0; i < nthreads; i++) {
		threads[i].id = 33333;
	}

	pause();

	return 0;
}
