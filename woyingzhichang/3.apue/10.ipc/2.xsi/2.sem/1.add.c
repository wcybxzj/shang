#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>

#define NUM 20
#define NAME "num.txt"
#define SIZE 10

void P(int semid){
	int ret;
	struct sembuf sf;
	sf.sem_num = 0;
	sf.sem_op = -1;
	sf.sem_flg = 0;
	//默认是条件不成熟阻塞等待，可以改成非阻塞
	while(semop(semid, &sf, 1)<0){
		if (errno == EINTR ||errno == EAGAIN) {
			continue;
		}else{
			perror("semop():");
		}
	}
	exit(-1);
}

void V(int semid){
	int ret;
	struct sembuf sf;
	sf.sem_num = 0;
	sf.sem_op = 1;
	sf.sem_flg = 0;
	//没做假做判断因为返回semarr数据一般直接还回去，
	//被阻塞从而引起假错的机会小
	ret = semop(semid, &sf, 1);
	if (ret < 0) {
		perror("semop():");
		exit(-1);
	}
}

static void stdio_func(int semid){
	char str[SIZE];
	int fd, num;
	FILE *fp = fopen(NAME, "r+");
	if(fp == NULL){
		perror("fopen():");
		exit(-1);
	}
	fd = fileno(fp);
	//lockf(fd, F_LOCK, 0);
	P(semid);
	fgets(str, SIZE, fp);
	num = atoi(str);
	num++;
	snprintf(str, SIZE, "%d",num);
	rewind(fp);
	//sleep(1);//放大冲突(可选)
	fputs(str, fp);
	fflush(NULL);//知识点
	V(semid);
	//lockf(fd, F_ULOCK, 0);
	fclose(fp);
	exit(0);
}

int fork_test_stdio(){
	int i, ret;
	pid_t pid;
	int semid;
	semid = semget(IPC_PRIVATE, 2, 0600);
	if(semid < 0){
		perror("semget():");
		exit(-1);
	}

	//写法1:
	//struct sembuf sf;
	//sf.sem_num = 0;
	//sf.sem_op = 1;
	//sf.sem_flg = 0;
	//ret = semop(semid, &sf, 1);
	//if (ret < 0) {
	//	perror("semop():");
	//	exit(-1);
	//}

	//写法2:
	if(semctl(semid, SETVAL, 1) < 0){
		perror("semctl()");
		exit(1);
	}


	for (i = 0; i < NUM; i++) {
		pid = fork();
		if (pid==0) {
			stdio_func(semid);
		}
	}
	for (i = 0; i < NUM; i++) {
		wait(NULL);
	}
	semctl(semid, 0, IPC_RMID);
}

//测试目的:
//使用IPC sem_arr 模拟一个多进程中使用的mutex,替代lockf文件锁实现多进程中对数据的同步
//测试方法:
//for i in {1..10}; do ./add; cat num.txt; done
int main(void){
	int fd;
	fd = open(NAME, O_CREAT|O_TRUNC|O_RDWR);
	write(fd, "0",1);

	fork_test_stdio();

	exit(0);
}
