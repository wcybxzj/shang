#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NUM 2000
#define NAME "num.txt"
#define SIZE 10

static void sysio_func(void *p){
	char tmp[SIZE]={0,};
	int fd;
	int num;
	fd = open(NAME, O_RDWR);
	if (fd < 0) {
		perror("open():");
		exit(-1);
	}
	lockf(fd, F_LOCK, 0);
	read(fd, tmp, SIZE);
	num = atoi(tmp);
	num++;
	sprintf(tmp,"%d", num);
	lseek(fd, SEEK_SET, 0);
	write(fd, tmp, strlen(tmp));
	lockf(fd, F_ULOCK, 0);
	close(fd);
	exit(0);
}

static void stdio_func(void *p){
	char str[SIZE];
	int fd, num;
	FILE *fp = fopen(NAME, "r+");
	if(fp == NULL){
		perror("fopen():");
		exit(-1);
	}
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0);
	fgets(str, SIZE, fp);
	num = atoi(str);
	num++;
	snprintf(str, SIZE, "%d",num);
	rewind(fp);
	fputs(str, fp);
	fflush(NULL);//知识点
	lockf(fd, F_ULOCK, 0);
	fclose(fp);
	exit(0);
}

int fork_test_sysio(){
	int i;
	pid_t pid;
	for (i = 0; i < NUM; i++) {
		pid = fork();
		if (pid==0) {
			sysio_func(NULL);
		}
	}
	for (i = 0; i < NUM; i++) {
		wait(NULL);
	}
}

int fork_test_stdio(){
	int i;
	pid_t pid;
	for (i = 0; i < NUM; i++) {
		pid = fork();
		if (pid==0) {
			stdio_func(NULL);
		}
	}
	for (i = 0; i < NUM; i++) {
		wait(NULL);
	}
}

int pthread_test_stdio(){
	int i;
	pthread_t tid[NUM];
	for (i = 0; i < NUM; i++) {
		pthread_create(tid+i, NULL, stdio_func, NULL);
	}
	for (i = 0; i < NUM; i++) {
		pthread_join(tid[i], NULL);
	}
}


//测试方法
//for i in {1..10}; do ./add; cat num.txt; done
int main(void){
	int fd;
	fd = open(NAME, O_CREAT|O_TRUNC|O_RDWR);
	write(fd, "0",1);
	//进程文件锁成功
	//fork_test_sysio();
	//fork_test_stdio();

	//多线程文件锁完全失败
	pthread_test_stdio();

	exit(0);
}
