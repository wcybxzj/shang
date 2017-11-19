#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#define FNAME "/tmp/123"

int setnonblocking( int fd )
{
	int old_option = fcntl( fd, F_GETFL );
	int new_option = old_option | O_NONBLOCK;
	fcntl( fd, F_SETFL, new_option );
	return old_option;
}

int is_nonblocking(int fd)
{
	int old_option = fcntl( fd, F_GETFL );
	return old_option&O_NONBLOCK;
}

//证明子进程继承了父进程打开文件的非阻塞的属性
int main(int argc, const char *argv[])
{
	pid_t pid;
	int fd = open(FNAME, O_CREAT, 0666);
	assert(fd >0);

	printf("父进程中fd的属性:%d\n", fcntl(fd, F_GETFL)& O_NONBLOCK);
	setnonblocking(fd);
	printf("父进程中fd的属性:%d\n", fcntl(fd, F_GETFL)& O_NONBLOCK);

	pid = fork();
	if (pid < 0) {
		perror("fork()");
		exit(1);
	}

	if (pid==0) {
		printf("子进程中fd是的属性:%d\n", fcntl(fd, F_GETFL)& O_NONBLOCK);
	}else if(pid > 0){

	}

	wait(NULL);

	exit(0);
}
