#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

void handler(int s){
	printf("current signal num is :%d\n", s);
}

int main(int argc, const char *argv[])
{
	pid_t pid;
	int status;

	signal(SIGCHLD, handler);

	pid = fork();
	if (pid==0) {
		exit(123);
	}

	wait(&status);
	if (WIFEXITED(status)) {
		printf("exit status %d\n", WEXITSTATUS(status));
	}

	if (WIFSTOPPED(status)) {
		printf("signal is %d\n", WSTOPSIG(status));
	}

	return 0;
}
