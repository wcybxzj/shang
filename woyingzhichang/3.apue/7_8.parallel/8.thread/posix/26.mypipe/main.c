#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "mypipe.h"
#include <sys/syscall.h>   

#define LEFT  30000000
#define RIGHT 30000200

#define THRNUM 4
#define WRITE_NUM 50
#define ARR_SIZE (RIGHT-LEFT+1)


//任务一次部署全部201个数
#define ALL_IN_ONE 1
//任务分成2次部署 第一次是101个数 第二次是100个数
#define ALL_IN_TWO 2
//任务分N次部署,每次WRITE_NUM次部署
#define ALL_IN_N 3


static void *writer(mypipe_t  *ptr, int op){
	int i, j, ret, index,count;
	int arr[ARR_SIZE];
	mypipe_register(ptr, MYPIPE_WRITE);
	switch(op){
		case ALL_IN_ONE:
			for(index=0,i=LEFT; i<=RIGHT; i++,index++){
				arr[index]=i;
			}
			ret = mypipe_write(ptr, arr, ARR_SIZE);
			if (ret <= 0) {
				mypipe_unregister(ptr, MYPIPE_WRITE);
			}
			break;
		case ALL_IN_TWO:
			for(index=0,i=LEFT; i<=30000100; i++,index++){
				arr[index]=i;
			}
			ret = mypipe_write(ptr, arr, index);
			//printf("index %d\n", index);
			if (ret < 0) {
				mypipe_unregister(ptr, MYPIPE_WRITE);
			}
			sleep(1);
			for(index=0,i=30000101; i<=RIGHT; i++,index++){
				arr[index]=i;
			}
			ret = mypipe_write(ptr, arr, index);
			//printf("index %d\n", index);
			if (ret < 0) {
				mypipe_unregister(ptr, MYPIPE_WRITE);
			}
			break;
		case ALL_IN_N:
			index = 0;
			for(i=LEFT; i<=RIGHT; i++){
				arr[index]=i;
				if(index == WRITE_NUM){
					ret = mypipe_write(ptr, arr, WRITE_NUM+1);
					if (ret <= 0) {
						mypipe_unregister(ptr, MYPIPE_WRITE);
					}
					index = 0;
					sleep(1);
					//printf("========================\n");
				}else{
					index++;
				}
				//printf("index:%d, i:%d\n", index, i);
			}
			if (index>0) {
				ret = mypipe_write(ptr, arr,index);
				if (ret <= 0) {
					mypipe_unregister(ptr, MYPIPE_WRITE);
				}
				index = 0;
				//sleep(1);
			}
			break;
		default:
			perror("writer() op:");
			break;
	}
}

void un_writer(mypipe_t  *ptr){
	mypipe_unregister(ptr, MYPIPE_WRITE);
	printf(">>>>writer finish<<<<< \n");
}

void cal(int i)
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
        printf("[%d]:%d is a primer.\n", syscall(SYS_gettid), i);
}

static void* func(void *ptr) 
{
	int i, ret;
	mypipe_register(ptr, MYPIPE_READ);
	while (1) {
		i=0;
		mypipe_read(ptr, &i, 1);
		//sleep(1);
		//printf("[%d]read from pipe:%d\n",\
				syscall(SYS_gettid),i);
		if(i == 0){
			printf("[%d]>>>reader finish<<<<\n",\
					syscall(SYS_gettid));
			pthread_exit(NULL);
		}
		cal(i); 
	}
}

int main(int argc, const char *argv[])
{
	int i, ret;
	mypipe_t *ptr; 
	pthread_t tid[THRNUM];

	ptr = mypipe_init();
	if (ptr == MYPIPE_INIT_FAIL) {
		perror("MYPIPE_INIT_FAIL:");
		exit(-1);
	}else if(ptr == Q_CREATE_FAIL){
		perror("Q_CREATE_FAIL:");
		exit(-1);
	}

	for(i=0; i<THRNUM; i++){
		ret = pthread_create(tid+i, NULL, func, (void*)ptr);
		if(ret){
			perror("pthread_create():");
			exit(0);
		}
	}

	//writer(ptr, ALL_IN_ONE);
	//writer(ptr, ALL_IN_TWO);
	writer(ptr, ALL_IN_N);

	sleep(1);//让reader有时间注册成功
	un_writer(ptr);

	for(i=0; i<THRNUM; i++){
		pthread_join(tid[i], NULL);
	}
	mypipe_destroy(ptr);
	exit(0);
}
