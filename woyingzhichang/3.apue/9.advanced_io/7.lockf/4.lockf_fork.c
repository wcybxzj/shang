#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NAME "num.txt"

//证明fork后子进程,不继承父亲中的锁
//方法1:ret = lockf(fd, F_TEST, 0);//测试
void child_lockf1(){
	pid_t pid;
	int fd, ret;

	fd = open(NAME, O_CREAT|O_TRUNC|O_RDWR,0777);
	ret = lockf(fd, F_LOCK, 0);
	if (ret < 0) {
		perror("lockf():");
		exit(-1);
	}

	pid = fork();
	if(pid == 0) {
		//close(fd);
		//fd = open(NAME,O_RDWR);
		//if (fd <0) {
		//	perror("child: open():");
		//	exit(-1);
		//}

		sleep(1);
		ret = lockf(fd, F_TEST, 0);//测试
		if(ret == 0){
			printf("child:被当前进程解锁或者上锁\n");
		}else if(ret < 0){
			printf("child:被其他进程锁住\n");
		}
		exit(0);
	}

	wait(NULL);
	ret = lockf(fd, F_TEST, 0);//测试
	if(ret == 0){
		printf("parent:被当前进程解锁或者上锁\n");
	}else if(ret < 0){
		printf("parent:被其他进程锁住\n");
	}
}

//证明fork后子进程,不继承父亲中的锁
//方法2:ret = lockf(fd, F_LOCK, 0);//测试
void child_lockf2(){
	pid_t pid;
	int fd, ret;

	fd = open(NAME, O_CREAT|O_TRUNC|O_RDWR,0777);
	ret = lockf(fd, F_LOCK, 0);
	if (ret < 0) {
		perror("lockf():");
		exit(-1);
	}

	pid = fork();
	if(pid == 0) {
		//close(fd);
		//fd = open(NAME,O_RDWR);
		//if (fd <0) {
		//	perror("child: open():");
		//	exit(-1);
		//}

		sleep(1);
		printf("child 上锁永远阻塞来证明,子进程没继承父进程的锁\n");
		ret = lockf(fd, F_LOCK, 0);//永远阻塞
		if(ret == 0){
			printf("child:被当前进程解锁或者上锁\n");
		}else if(ret < 0){
			printf("child:被其他进程锁住\n");
		}
		exit(0);
	}
	wait(NULL);
}

//证明父子进程共享文件表项
void child_lseek(){
	pid_t pid;
	char str[100]={};
	int fd;
	fd = open("/etc/services", O_RDONLY);
	pid = fork();

	if (pid==0) {
		//跳过开头的#,影响到父进程的fd
		lseek(fd, 1, SEEK_SET);
		exit(0);
	}
	pause();
	wait(NULL);
	read(fd, str, 3);
	printf("%s\n", str);
}

int main(void){
	//child_lockf1();
	child_lockf2();
	//child_lseek();
	exit(0);
}
