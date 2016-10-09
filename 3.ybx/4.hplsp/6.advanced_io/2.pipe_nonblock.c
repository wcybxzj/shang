#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

static void sig_handler(int s){
	if (s==13) {
		printf("SIGPIPE\n");
	}
}

//pipe非阻塞情况中
//子进程sleep(1) 保证父进程如果有动作先运行
//
//如果父进程close pd[1],子进程close pd[1],
//pd[1]引用计数为0,管道没有数据,子进程read EOF
//
//如果父进程没close pd[1]
//pd[1] 引用计数为1,管道没数据,子进程read EAGAIN
void read_nonblock(int fd[])
{
    char buf[BUFSIZ] = "";
	int len;
	pid_t pid;
	pid = fork();
	if (pid==0) {
		sleep(1);
		close(fd[1]);
		len = read(fd[0], buf, BUFSIZ);
		printf("len:%d\n", len);
		if (len==-1) {
			if (errno == EAGAIN) {
				printf("EAGAIN\n");
				exit(-1);
			}
		}else if(len==0){
			printf("EOF\n");
		}
		exit(0);
	}else{
		//close(fd[1]);
		wait(NULL);
	}
}

//如果父进程只是wait, 因为pd[0]引用数不为0
//子进程write成功
//
//如果父进程close pd[0],子进程close pd[0],pd[0]引用数为0
//子进程write pd[1]返回-1,errno是EPIPE,引起信号SIGPIPE
void write_nonblock(int fd[])
{
    char buf[BUFSIZ] = "abc";
    char buf2[BUFSIZ];
	int len;
	pid_t pid;

	pid = fork();
	if (pid==0) {
		close(fd[0]);
		signal(SIGPIPE, sig_handler);
		sleep(1);
		len = write(fd[1], buf, BUFSIZ);
		if (len==-1) {
			if (errno == EPIPE) {
				printf("EPIPE\n");
			}
		}else if(len > 0){
			printf("write success len:%d\n",len);
		}
		exit(0);
	}else{
		close(fd[0]);
		wait(NULL);
	}
}

int main (void)
{
	int pd[2];
	if(pipe(pd) < 0){
		perror("perror():");
		exit(0);
	}

	int fd1_save = fcntl(pd[0], F_GETFL);
	int fd2_save = fcntl(pd[1], F_GETFL);
	fcntl(pd[0], F_SETFL, fd1_save|O_NONBLOCK);
	fcntl(pd[1], F_SETFL, fd1_save|O_NONBLOCK);

	//read_nonblock(pd);
	write_nonblock(pd);
	return 0;
}


