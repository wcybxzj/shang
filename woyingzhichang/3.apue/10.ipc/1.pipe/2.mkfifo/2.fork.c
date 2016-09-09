#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1024
#define NAME "/tmp/fifo.txt"
//fork 创建2个无亲缘关系的进程, 2个进程用同一个命名管道互相读写一次
//非典型管道用法
int main(void)
{
	int ret, fd, len;
	char buf[SIZE];
	pid_t pid;

	unlink(NAME);
	ret = mkfifo(NAME, 0644);
	if (ret < 0) {
		perror("mkfifo():");
		exit(-1);
	}

	pid = fork();
	if(pid>0){
		pid = fork();
		if(pid>0){
			printf("parent pid:%d, ppid:%d\n", getpid(), getppid());
			exit(0);
		}else{
			sleep(1);
			printf("child2 pid:%d, ppid:%d\n", getpid(), getppid());
			fd = open(NAME, O_RDWR);
			printf("block\n");
			len = read(fd, buf, SIZE);
			printf("unblock\n");
			printf("child2:%s\n",buf);
			write(fd,"456",3);
			close(fd);
			printf("child2 end\n");
			exit(0);
		}
	}else{
		sleep(1);
		printf("child1 pid:%d, ppid:%d\n", getpid(), getppid());
		fd = open(NAME, O_RDWR);
		write(fd,"123", 4);
		//sleep等child 2 read,否则child 1自己写自己读,child 2读不到数据阻塞在read
		sleep(1);
		len = read(fd, buf, SIZE);
		printf("child1:%s\n",buf);
		close(fd);
		printf("child1 end\n");
		exit(0);
	}
	exit(0);
}
