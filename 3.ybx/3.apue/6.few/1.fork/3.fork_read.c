#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

#define MAX_SIZE 1024

//测试1:父子进程同时对标准输入读取会怎么样
//
/*
结果:只能有1个进程读取到内容,不一定是哪个进程

[root@web11 1.fork]# ./3.fork_read 
aaaa
pid:9126, content:aaaa

bbbb
pid:9127, content:bbbb

aaaa
pid:9127, content:aaaa

ccc
pid:9127, content:ccc
*/

void test1()
{
	int ret;
	char buf[MAX_SIZE];
	bzero(buf, sizeof(buf));
	pid_t pid;
	pid = fork();
	if (pid<0) {
		exit(1);
	}else if(pid == 0){
		while (1) {
			read(STDIN_FILENO, buf, MAX_SIZE);
			printf("pid:%d, content:%s\n", getpid(), buf);
		}
		exit(0);
	}else{
		while (1) {
			read(STDIN_FILENO, buf, MAX_SIZE);
			printf("pid:%d, content:%s\n", getpid(), buf);
		}
		wait(NULL);
	}
}

int main(int argc, const char *argv[])
{
	test1();
	return 0;
}
