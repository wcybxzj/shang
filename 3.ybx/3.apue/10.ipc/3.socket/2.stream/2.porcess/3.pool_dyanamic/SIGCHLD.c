#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//资料tlpi26.3.3
//选项1或者选项2 
//任何一个都可以完成子进程不用收尸,不会让子进程成为僵尸
void func(){
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;//选项1
	sigemptyset(&sa.sa_mask);
	//sa.sa_flags = SA_NOCLDWAIT;//选项2
	sigaction(SIGCHLD, &sa, NULL);
}

//测试:
//使用func
//kill -TERM 子进程pid
//子进程不会变成僵尸,直接被kill
//不使用func
//子进程成为僵尸进程
int main(int argc, const char *argv[])
{
	pid_t pid;
	//func();
	pid = fork();
	if(pid == 0){
		printf("child process id is:%d\n",\
				getpid());
		sleep(1000);
		exit(0);
	}

	sleep(1000);
	return 0;
}
