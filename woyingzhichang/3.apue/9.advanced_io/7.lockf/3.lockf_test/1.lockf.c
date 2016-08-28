#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NAME "num.txt"

void func(void){
	printf("current pid is %d\n", getpid());
	int fd, fd2, num,ret;
	fd = open(NAME, O_RDWR);
	if(fd < 0){
		perror("open():");
		exit(-1);
	}
	ret = lockf(fd, F_LOCK, 0);
	if (ret==0) {
		printf("%s\n", "lock by current process");
		printf("%s\n", "第一sleep 10,在此期间是LOCK状态");
		sleep(10);
		printf("%s\n", "sleep over");
	}

	fd2 = open(NAME, O_RDWR);
	//fd2 = dup(fd);
	close(fd2);

	printf("%s\n","第二次sleep 10,此状态是意外解除前面LOCK");
	sleep(10);
	printf("%s\n", "sleep over");

	lockf(fd, F_ULOCK, 0);
	exit(0);
}

//先运行
int main(void){
	int fd;
	fd = open(NAME, O_CREAT|O_TRUNC|O_RDWR,0777);
	write(fd, "0",1);
	func();
	exit(0);
}
