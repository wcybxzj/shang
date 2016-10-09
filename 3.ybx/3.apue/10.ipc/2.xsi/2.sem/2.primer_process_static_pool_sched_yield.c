#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>

#define LEFT  30000000
#define RIGHT 30000200
#define NUM (RIGHT-LEFT)

#define PROCESS_NUM 5
#define INT_SIZE (sizeof(char)*4)

void cal(int i);
void P(int semid);
void V(int semid);

void func(int **num, int semid){
	int i;
	while (1) {
		//printf("pid:%d, num:%d\n", getpid(), *(*num));
		P(semid);
		while (*(*num) == 0) {
			V(semid);
			sched_yield();
			P(semid);
		}
		if (*(*num) == -1) {
			V(semid);
			break;
		}
		i = *(*num);
		*(*num) = 0;
		V(semid);
		cal(i);
	}
	exit(0);
}

int main(int argc, const char *argv[])
{
	int i, err;
	int *num;
	pid_t pid;
	int semid;

	num = mmap(0, INT_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if(num == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}   

	if(semctl(semid, 0,SETVAL, 1) < 0){
		perror("semctl()");
		exit(1);
	}

	for (i = 0; i < PROCESS_NUM; i++) {
		pid = fork();
		if (pid == 0) {
			func(&num, semid);
		}
	}

	for (i = LEFT; i <= RIGHT; i++) {
		P(semid);
		while (*num != 0) {
			V(semid);
			sched_yield();
			P(semid);
		}
		*num = i;
		V(semid);
	}

	P(semid);
	while (*num != 0) {
		V(semid);
		sched_yield();
		P(semid);
	}
	*num=-1;
	V(semid);

	for (i = 0; i < PROCESS_NUM; i++) {
		wait(NULL);
	}

	return 0;
}

void cal(int i)
{
    int j, mark;
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
	{
		printf("[%d]%d is a primer.\n", \
			getpid(), i);
	}
}

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
