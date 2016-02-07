#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

//虽然看似 两个进程地址一样,其实他们根本不在一个虚拟内存空间上
int main(int argc, const char *argv[])
{
	int tmp = 123;
	pid_t pid;
	pid = fork();
	if (pid<0) {
		perror("fork():");
		exit(1);
	}

	if (pid > 0) {
		printf("pid:%d, tmp:address:%X\n", getpid(), &tmp);
	}else{
		printf("pid:%d, tmp:address:%X\n", getpid(), &tmp);
	}

	return 0;
}
