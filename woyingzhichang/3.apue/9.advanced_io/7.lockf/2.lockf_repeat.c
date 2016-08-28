#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NAME "num.txt"

void func(void){
	int fd, num,ret;
	FILE *fp = fopen(NAME, "r+");
	if(fp == NULL){
		perror("fopen():");
		exit(-1);
	}
	fd = fileno(fp);
	ret = lockf(fd, F_LOCK, 0);
	if (ret==0) {
		printf("%s\n","one lockf ok");
	}
	ret = lockf(fd, F_LOCK, 0);
	if (ret==0) {
		printf("%s\n", "two lockf ok");
	}
	lockf(fd, F_ULOCK, 0);
	fclose(fp);
	exit(0);
}

//测试同进程连续lockf
int main(void){
	int fd;
	fd = open(NAME, O_CREAT|O_TRUNC|O_RDWR);
	write(fd, "0",1);
	func();
	exit(0);
}
