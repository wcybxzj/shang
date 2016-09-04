#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define NUM 2

union semun
{
	int val;    //           <= value for SETVAL
	struct semid_ds *buf; //     <= buffer for IPC_STAT & IPC_SET
	unsigned short int *array; //    <= array for GETALL & SETALL
	struct seminfo *__buf;    // <= buffer for IPC_INFO
};


void display_semarr(int semid){
	int i;
	union semun arg1;
	arg1.array = calloc(2, sizeof(arg1.array[0]));
	if (arg1.array == NULL) {
		perror("calloc()");
		exit(-1);
	}
	if(semctl(semid, /*when GETALL ignore*/0, \
				GETALL, arg1) < 0){
		perror("semctl()");
		exit(1);
	}
	for(i=0; i<2; i++){
		printf("get %d\n",arg1.array[i]);
	}
	free(arg1.array);
}

void P(int semid, int process_resource[][2], int process_num){
	int ret;
	struct sembuf sf_arr[2];
	printf("%d ,%d\n", \
			process_resource[process_num][0], \
			process_resource[process_num][1]);
	sf_arr[0].sem_num = 0;
	sf_arr[0].sem_op = -process_resource[process_num][0];
	sf_arr[0].sem_flg = 0;
	sf_arr[1].sem_num = 1;
	sf_arr[1].sem_op = -process_resource[process_num][1];
	sf_arr[1].sem_flg = 0;

	//默认是条件不成熟阻塞等待，可以改成非阻塞
	while(semop(semid, sf_arr, 2)<0){
		if (errno == EINTR ||errno == EAGAIN) {
			continue;
		}else{
			perror("semop():");
			exit(-1);
		}
	}
}

void V(int semid, int process_resource[][2], int process_num){
	int ret;
	struct sembuf sf_arr[2];
	sf_arr[0].sem_num = 0;
	sf_arr[0].sem_op = process_resource[process_num][0];
	sf_arr[0].sem_flg = 0;
	sf_arr[1].sem_num = 1;
	sf_arr[1].sem_op = process_resource[process_num][1];
	sf_arr[1].sem_flg = 0;

	//默认是条件不成熟阻塞等待，可以改成非阻塞
	while(semop(semid, sf_arr, 2)<0){
		if (errno == EINTR ||errno == EAGAIN) {
			continue;
		}else{
			perror("semop():");
			exit(-1);
		}
	}
}

static void stdio_func(int semid, int process_resource[][2], \
		int process_num){
	printf("parent pid:%d, ppid:%d\n", getpid(), getppid());
	printf("%d, block\n", process_num);
	P(semid,  process_resource, process_num);
	printf("%d, unblock\n", process_num);
	sleep(5);
	V(semid,  process_resource, process_num);
	exit(0);
}

//fork 创建的是非亲缘关系的两个子进程来做IPC
int fork_test_stdio(){
    struct semid_ds ds;
	union semun arg;
	int i, ret;
	pid_t pid;
	int semid;
	int init_resource[2] = {5 ,5};//初始资源  A 5,B 5
	int process_resource[2][2] = {
		{10, 5}, //进程1 A需要10 B需要5
		{3,5}, //进程2 A需要3 B需要5
	};
	semid = semget(IPC_PRIVATE, 2, 0600);
	if(semid < 0){
		perror("semget():");
		exit(-1);
	}

	//printf("semid is %d\n", semid);

	arg.array = calloc(2, sizeof(arg.array[0]));
	if (arg.array == NULL) {
		perror("calloc()");
		exit(-1);
	}

	for(i=0; i<2; i++){
        arg.array[i] = init_resource[i];
	}

	if(semctl(semid, /*when SETALL ignore*/0, \
				SETALL, arg) < 0){
		perror("semctl()");
		exit(1);
	}

	display_semarr(semid);

	for (i=0; i < NUM; i++) {
		pid = fork();
		if (pid==0) {
			stdio_func(semid, process_resource, i);
			exit(0);
		}
	}

	display_semarr(semid);
	exit(0);
}

//测试目的:
//使用IPC sem_arr 模拟一个多进程中使用的mutex,替代lockf文件锁实现多进程中对数据的同步
//测试方法:
//for i in {1..10}; do ./add; cat num.txt; done
int main(void){
	fork_test_stdio();
	exit(0);
}
