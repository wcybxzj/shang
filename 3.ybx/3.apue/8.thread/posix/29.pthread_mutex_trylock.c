#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
pid_t gettid()
{
     return syscall(SYS_gettid);
}

pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;

static void* func(void *p){
	int i, err;
	while ( (err = pthread_mutex_trylock(&mut_num))) {
		printf("tid:%d\n",gettid());
		printf("errno:%s\n", strerror(err));//EBUSY
		sleep(1);
	}
	for (i = 0; i < 10; i++) {
		printf("tid:%d,i:%d\n", gettid(), i);
		sleep(1);
	}
	err = pthread_mutex_unlock(&mut_num);
}

//1.测试下pthread_mutex_trylock 非阻塞上锁
//2.看到pthread_mutex_trylock 返回的错误是直接返回的而不是使用全局的errno
int main(int argc, const char *argv[])
{
	int err;
	void *res;
	pthread_t tid[2];
	int i;
	for (i = 0; i < 2; i++) {
		pthread_create(&tid[i], NULL, func, NULL);
	}

	//for (i = 0; i < 2; i++) {
	//	pthread_join(tid[i], res);
	//}

	pause();
	return 0;
}
