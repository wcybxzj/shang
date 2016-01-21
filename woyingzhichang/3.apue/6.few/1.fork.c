#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{

	pid_t pid;
	//现象3:
	//./1.fork
	//出现1个BEGIN
	//
	//1.fork > /tmp/out
	//出现2个BEGIN，因为文件是全缓冲
	printf("[%d]Begin!\n" , getpid());
	//fflush(NULL);//fork 前要刷新缓冲区

	pid = fork();
	if (pid < 0) {
		perror("fork()");
		exit(1);
	}

	if (pid==0) {
		//现象1:
		//父进程结束,然后显示出命令行后才显示子进程的效果
		//sleep(1);//为了在我的机器上,实现整个效果
		printf("[%d]Child!\n" , getpid());
	}else if(pid > 0){
		printf("[%d]Parent!\n" , getpid());
	}

	//现象2:
	//ps axf 查看进程关系
	//注意要在终端输入2次内容，因为getchar在父子两个进程中
	//getchar();

	printf("[%d]END!\n" , getpid());
	exit(0);
}
