#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


static void* func(void*ptr)
{
	puts("Thread is working!!");
	pthread_exit(123);
	//return NULL;
}

int main(int argc, const char *argv[])
{
	int err;
	pthread_t tid;
	puts("begin");
	err = pthread_create(&tid, NULL, func, NULL );
	if (err) {
		fprintf(stderr, "%s\n", strerror(err));
		exit(1);
	}

	//sleep(1);
	pthread_join(tid, NULL);//阻塞等待的系统调用
	puts("end");
	return 0;;
}
