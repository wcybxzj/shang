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

void func()
{
	int i;
	int shmid;
	pid_t pid;
	//都可以
	//shmid = shmget(KEY, SIZE, IPC_CREAT|0666);//命名IPC
	shmid = shmget(IPC_PRIVATE, SIZE, 0666);//匿名IPC
	printf("shmid is:%d\n", shmid);
	char *str;
	char *buf="nice to see you";
	for(i=0; i<2; i++){
		pid = fork();
		if(pid==0){
			printf("pid:%d, ppid:%d\n", getpid(), getppid());
			if(i==0){//child 1
				str = shmat(shmid, NULL, 0);
				if(str == (void*)(-1)){
					perror("shmat():");
					exit(-1);
				}
				strcpy(str, buf);
				shmdt(str);
				exit(0);
			}else if(i==1){//child2
				str = shmat(shmid, NULL, 0);
				if(str == (void*)(-1)){
					perror("shmat():");
					exit(-1);
				}
				printf("child 2 get:%s", str);
				shmctl(shmid, IPC_RMID,NULL);
				shmdt(str);
				exit(0);
			}
			exit(0);
		}
	}
	for (i = 0; i < 2; i++) {
		wait(NULL);
	}
	exit(0);
}


//有亲缘关系的进程之间通信
int main(void){
	func();
	exit(0);
}
