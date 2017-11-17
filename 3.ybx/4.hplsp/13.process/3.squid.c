#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define FNAME "/tmp/1.txt"


//测试1:
//父子进程之间使用socketpair进行通信
/*
[root@web11 13.process]# ./3.squid
parent pid:11369
ybx
send ok
recv ok, recv content:ybx, len:3
*/
void test()
{
	int fd, len;
	pid_t pid;
	int pipefd[2];
	int ret;
	char send_str[20];
	char recv_str[20];

	ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
	if (ret==-1) {
		printf("socketpair()\n");
		exit(1);
	}

	pid = fork();
	//子进程:
	if (pid==0) {
		close(pipefd[1]);
		while (1) {
			bzero(recv_str, sizeof(recv_str));
			ret = recv(pipefd[0], recv_str, sizeof(recv_str), 0);
			if (ret == -1) {
				printf("recv error\n");
				exit(1);
			}
			else{
				printf("recv ok, recv content:%s, len:%d\n",recv_str, strlen(recv_str));
			}
		}
		printf("child pid:%d\n", getpid());
		exit(1);
	}

	//父进程:
	close(pipefd[0]);
	printf("parent pid:%d\n", getpid());
	while (1) {
		bzero(send_str, sizeof(send_str));
		ret = read(STDIN_FILENO, send_str, sizeof(send_str));
		if (ret==-1) {
			printf("read error\n");
			exit(1);
		}else{
			len = strlen(send_str)-1;
			send_str[len] = '\0';
		}
		ret = send(pipefd[1], ( char* )&send_str, strlen(send_str), 0 );
		if (ret == -1) {
			printf("send error\n");
			exit(1);
		}else{
			printf("send ok\n");
		}
	}
	wait(NULL);
}

//测试2:模拟hplsp page 242上的例子
/*
测试方法:
[root@web11 13.process]# ./3.squid
parent pid:11297
ybx
send ok
wc
send ok
*/
void squid()
{
	int fd, len;
	pid_t pid;
	int pipefd[2];
	int ret;
	char send_str[20];
	char recv_str[20];

	ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
	if (ret==-1) {
		printf("socketpair()\n");
		exit(1);
	}

	pid = fork();
	if (pid==0) {
		close(pipefd[1]);
		fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0660);
		if (fd == -1) {
			perror("open():");
			exit(0);
		}

		dup2(pipefd[0],STDIN_FILENO);
		dup2(pipefd[0],STDOUT_FILENO);
		close(pipefd[0]);
		while (1) {
			bzero(recv_str, sizeof(recv_str));
			//ret = recv(pipefd[0], recv_str, sizeof(recv_str), 0 );
			ret = recv(STDIN_FILENO, recv_str, sizeof(recv_str), 0 );
			if (ret == -1) {
				printf("recv error\n");
				exit(1);
			}
			else{
				printf("recv ok, recv content is:%s\n",recv_str);
				write(fd, recv_str, strlen(recv_str));
			}
		}
		printf("child pid:%d\n", getpid());
		exit(1);
	}

	//父进程:
	close(pipefd[0]);
	printf("parent pid:%d\n", getpid());
	while (1) {
		bzero(send_str, sizeof(send_str));
		ret = read(STDIN_FILENO, send_str, sizeof(send_str));
		if (ret==-1) {
			printf("read error\n");
			exit(1);
		}else{
			len = strlen(send_str)-1;
			send_str[len] = '\0';
		}
		ret = send(pipefd[1], ( char* )&send_str, strlen(send_str), 0 );
		if (ret == -1) {
			printf("send error\n");
			exit(1);
		}else{
			printf("send ok\n");
		}
	}
	wait(NULL);
}

int main(int argc, const char *argv[])
{
	test();
	//squid();
	return 0;
}
