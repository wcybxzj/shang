#include <stdio.h>
#include <pthread.h>
#include <string.h>
#define NUM 1000

static pthread_t tids[NUM];
static void* func(void *ptr) {
	sleep(300);//开启测试占用资源,关闭测试创建速度
	pthread_exit(NULL);
}

int main(void){
	int i, err;
	for(i=0; i<NUM; i++){
		err = pthread_create(tids+i, NULL, func, NULL);
		if(err){
			fprintf(stderr, "pthread_create():%s", strerror(err));
		}
	}
	for(i=0; i<NUM; i++){
		pthread_join(tids[i], NULL);

	}
	return 0;
}

