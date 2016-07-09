#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#define MAX 2

static void* func(void *ptr)
{
	int i=123;
	printf("%x\n", &i);
	while (1) {
		pause();
	}
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	int i,err;
	int size= 1024*10240;

	size_t stacksize;

	//pthread_attr_t attr;
	//pthread_attr_init(&attr);
	//err = pthread_attr_setstacksize(&attr, size);//100m
	//if (err) {
	//	printf("ddddd\n");
	//	exit(0);
	//}
	//err = pthread_attr_getstacksize(&attr, &stacksize);//100m
	//printf("%d", stacksize);

	pthread_t tids[MAX];
	for (i = 0; i < MAX; i++) {
		//err = pthread_create(tids+i, &attr, func, NULL);
		err = pthread_create(tids+i, NULL, func, NULL);
		if (err) {
			fprintf(stderr, "%s\n", strerror(err));
		}
		printf("%d\n",i);
	}

	for (i = 0; i < MAX; i++) {
		pthread_join(tids[i], NULL);
	}

	return 0;
}


