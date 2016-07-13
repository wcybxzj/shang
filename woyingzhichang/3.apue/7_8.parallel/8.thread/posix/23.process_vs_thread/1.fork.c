#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define NUM 1000

//time ./fork 
//finish
//real	0m5.654s
//user	0m0.063s
//sys	0m2.589s

static pid_t pid_arr[NUM];
int main(void){
	int i;
	pid_t pid;
	for(i=0; i<NUM; i++){
		pid = fork();
		pid_arr[i] = pid;
		if(pid < 0){
			perror("fork()");
		}
		if(pid == 0){
			sleep(300);//开启测试占用资源,关闭测试创建速度
			exit(0);
		}
	}

	for(i=0; i<NUM; i++){
		wait(NULL);
	}
	
	printf("finish");

	exit(0);
}
