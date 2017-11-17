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


int main(int argc, const char *argv[])
{
	int shmid;
	shmid = shmget(KEY, SIZE, IPC_CREAT|0666);//命名IPC
	printf("shmid is:%d\n", shmid);
	char *str;
	str = shmat(shmid, NULL, 0);
	if(str == (void*)(-1)){
		perror("shmat():");
		exit(-1);
	}
	printf("child 2 get:%s", str);
	shmctl(shmid, IPC_RMID,NULL);
	shmdt(str);
	exit(0);
	return 0;
}
