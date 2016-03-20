#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


static void* func(void*ptr)
{
	int abc =123;
	puts("Thread is working!!");
	pthread_exit(&abc);
	//return NULL;
}

int main(int argc, const char *argv[])
{
	int err;
	pthread_t tid;
	int *ret;
	puts("begin");
	err = pthread_create(&tid, NULL, func, NULL );
	if (err) {
		fprintf(stderr, "%s\n", strerror(err));
		exit(1);
	}

	pthread_join(tid, &ret);//阻塞等待的系统调用
	printf("ret is %d\n", *(*ret));
	puts("end");
	return 0;;
}
