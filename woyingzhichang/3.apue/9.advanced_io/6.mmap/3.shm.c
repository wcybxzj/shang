#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FNAME "/etc/services"

#define SIZE 1024


//进程带亲缘关系的进程间通讯
int main(void){
	char *str;
	pid_t pid;
	str = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if(str == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}

	pid = fork();
	if (pid==0) {//child
		printf("child addr is :%p\n", str);
		//str="hello";//error,因为父子成内存空间是独立的
		strcpy(str, "hello");//必须写到共享内存中父进程才能取到
		munmap(str, SIZE);
		exit(0);
	}else if(pid <0){
		perror("fork():");
		exit(1);
	}
	wait(NULL);
	printf("parent addr is :%p\n", str);
	printf ("parent  str is :%s\n", str);
	munmap(str, SIZE);
	exit(0);
}
