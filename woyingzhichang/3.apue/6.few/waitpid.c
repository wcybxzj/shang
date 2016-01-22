#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int i, status;
	pid_t pid, tmp_pid;
	for (i = 0; i < 10; i++) {
		pid = fork();
		if (pid < 0) {
			exit(1);
		}
		if (pid == 0) {
			printf("child is %d\n", getpid());
			//sleep(2);
			exit(12);
		}
	}

	//tmp_pid = waitpid(-1, NULL, WNOHANG);
	//printf("%d\n", (int)tmp_pid);

	while (tmp_pid = waitpid(-1, &status, WNOHANG)!=-1) {
		printf("%d %d\n", (int)tmp_pid , status);//status　为什么不对, 输出也不稳定奇怪
	}
	printf("parent finished\n");
	return 0;
}
