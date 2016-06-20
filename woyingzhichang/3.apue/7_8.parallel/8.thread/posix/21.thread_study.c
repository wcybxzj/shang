#include <stdio.h>
#include <pthread.h>
#define NUM 10

static void* func(void *p){
	int i = (int)p;
	printf("little brother:%d\n", i);
	sleep(6);
	pthread_exit((void*)i);
	//return (void *) i;
}

//小弟thread sleep
//大哥thread sleep
int main(int argc, const char *argv[])
{
	pthread_t tid[NUM];
	int i,err;
	void *ret;
	for (i = 0; i < NUM; i++) {
		pthread_create(tid+i, NULL, func, (void *)i);
	}

	sleep(10);
	for (i = 0; i < NUM; i++) {
		printf("big brother:%d\n", i);
		err = pthread_join(tid[i], &ret);
		if (err) {
			printf("pthread fail: %s\n", strerror(err));
		}else{
			printf("%s\n", "pthread ok");
			printf("ret is %d\n",(int)ret);
		}
	}
	return 0;
}
