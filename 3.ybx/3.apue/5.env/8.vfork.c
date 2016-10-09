#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

//fork:
int main(int argc, const char *argv[])
{
	pid_t pid;
	int status;
	int i;
	pid = fork();
	if (pid==0) {
		sleep(1000);
	}else{
		wait(&status);
		if (WIFEXITED(status)) {
			printf("exit status %d\n", WEXITSTATUS(status));
		}
		if (WIFSTOPPED(status)) {
			printf("signal is %d\n", WSTOPSIG(status));
		}
	}
	return 0;
}
