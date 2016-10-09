#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define SIZE 1024

void func(int sig){
	if (sig==13) {
		printf("SIGPIPE\n");
	}
}

//pipe阻塞情况中
//子进程sleep(1) 保证父进程如果有动作先运行
//
//如果父进程close pd[1],子进程也close pd[1] 
//pd[1]引用数0,管道中没有数据,子进程read EOF
//
//如果父进程没close pd[1] 
//pipe中没数据但pd[1]引用数量不为0,子进程read阻塞等输入内容
void read_block(int pd[])
{
	int pid;
	int len;
	char str[SIZE];
	pid = fork();
	if(pid == 0){
		close(pd[1]);
		sleep(1);//让对端有机会先close pd[1]
		len = read(pd[0], str, SIZE);
		if (len==0) {
			printf("EOF\n");
		}else{
			printf("child read from pipe:%s\n", str);
		}
		exit(0);
	}else if(pid >0){
		//close(pd[1]);
		wait(NULL);
	}
}

//如果父进程只是wait, 因为pd[0]引用数不为0
//子进程write成功
//
//如果父进程close pd[0],子进程close pd[0],pd[0]引用数为0
//子进程write pd[1]返回-1,errno是EPIPE,引起信号SIGPIPE
void write_block(int pd[]){
	int pid;
	int len;
	char str[SIZE];
	signal(SIGPIPE,func);
	pid = fork();
	if(pid == 0){
		close(pd[0]);
		sleep(1);//让对端有机会先close pd[0]
		len = write(pd[1],"abc!", 5);
		if (len==-1) {
			if (errno==EPIPE) {
				printf("EPIPE\n");
			}
		}else if(len >0){
			printf("write success len:%d\n", len);
		}
		exit(0);
	}else if(pid >0){
		//close(pd[0]);
		wait(NULL);
	}
}

int main(){
	int pd[2];
	if(pipe(pd) < 0){
		perror("perror():");
		exit(0);
	}

	//read_block(pd);
	write_block(pd);
	
	exit(0);
}
