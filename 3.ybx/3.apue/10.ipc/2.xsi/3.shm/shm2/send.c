#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#define SIZE 1024
#define KEY 0x12345678



//使用方法:
//演示无亲缘关系进程间使用shm进行通信
//./send
//./recv
int main(void){
	int shmid;
	shmid = shmget(KEY, SIZE, IPC_CREAT|0666);//命名IPC
	printf("shmid is:%d\n", shmid);
	char *str;
	char *buf="nice to see you";
	printf("pid:%d, ppid:%d\n", getpid(), getppid());
	str = shmat(shmid, NULL, 0);
	if(str == (void*)(-1)){
		perror("shmat():");
		exit(-1);
	}
	strcpy(str, buf);
	shmdt(str);
	exit(0);
}
