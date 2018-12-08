#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

//实验:
//echo ybx > wo.txt

/*
测试1:使用FD_CLOEXEC

输出:
6.FD_CLOEXEC:child, bytes:1, y
7.exec: read fail:: Bad file descriptor
6.FD_CLOEXEC:parent, bytes:3, bx

现象1:父进程打开的文件获取fd,fork子进程后进程fd。此时父子进程的fd共享同一个文件表,文件表中有当前此fd的偏移量
现象2:使用FD_CLOEXEC后exec中的进程无法读取到数据
*/
void test1 ()
{
	int fd, pid;
	char buffer[20];
	fd = open("wo.txt", O_RDONLY);
	int val = fcntl(fd, F_GETFD);
	val = val|FD_CLOEXEC;//重点
	fcntl(fd, F_SETFD, val);

	pid = fork();
	if(pid == 0)
	{
		//子进程中，此描述符并不关闭，仍可使用
		char child_buf[2];
		memset(child_buf, 0, sizeof(child_buf));
		ssize_t bytes = read(fd, child_buf, sizeof(child_buf)-1);
		printf("6.FD_CLOEXEC:child, bytes:%ld, %s\n", bytes, child_buf);

		//execl执行的程序里，此描述符被关闭，不能再使用它
		char fd_str[5];
		memset(fd_str, 0, sizeof(fd_str));
		sprintf(fd_str, "%d", fd);
		int ret = execl("./7.exec", "7.exec", fd_str, NULL);
		if(-1 == ret)
			perror("ececl fail:");

		exit(0);
	}

	sleep(1);
	waitpid(pid,NULL,0);
	memset(buffer,0,sizeof(buffer) );
	ssize_t bytes = read(fd,buffer,sizeof(buffer)-1 );
	printf("6.FD_CLOEXEC:parent, bytes:%ld, %s\n", bytes, buffer);
}

//测试2:不使用FD_CLOEXEC

/*
输出:
child, bytes:1, y
7.exec: read 3, bx  ---->可以看到exec后能读出内容
6.FD_CLOEXEC:parent, bytes:0, 
*/
void test2 ()
{
	int fd, pid;
	char buffer[20];
	fd = open("wo.txt", O_RDONLY);

	pid = fork();
	if(pid == 0)
	{
		//子进程中，此描述符并不关闭，仍可使用
		char child_buf[2];
		memset(child_buf, 0, sizeof(child_buf));
		ssize_t bytes = read(fd, child_buf, sizeof(child_buf)-1);
		printf("6.FD_CLOEXEC:child, bytes:%ld, %s\n", bytes, child_buf);

		//execl执行的程序里，此描述符被关闭，不能再使用它
		char fd_str[5];
		memset(fd_str, 0, sizeof(fd_str));
		sprintf(fd_str, "%d", fd);
		int ret = execl("./7.exec", "7.exec", fd_str, NULL);
		if(-1 == ret)
			perror("ececl fail:");

		exit(0);
	}

	sleep(1);
	waitpid(pid,NULL,0);
	memset(buffer,0,sizeof(buffer) );
	ssize_t bytes = read(fd,buffer,sizeof(buffer)-1 );
	printf("6.FD_CLOEXEC:parent, bytes:%ld, %s\n", bytes, buffer);
}

int main(void)
{
	//test1();
	test2();
}

