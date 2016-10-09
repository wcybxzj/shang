#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	pid_t pid;
	printf("BEGIN\n");
	fflush(NULL);
	pid = fork();
	if (pid<0) {
		perror("fork()");
		exit(0);
	}

	if (pid == 0) {
		execl("/bin/sleep", "我的黑客程序哈哈", "100",NULL);
		perror("execl():");
		exit(1);
	}

	wait(NULL);
	printf("END\n");
	exit(0);
}
