#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int i, status;
	pid_t pid, tmp_pid;
	//10个子进程有10种结束状态
	for (i = 0; i < 10; i++) {
		pid = fork();
		if (pid < 0) {
			exit(1);
		}
		if (pid == 0) {
			printf("child is %d\n", getpid());
			//sleep(2);
			exit(i);
		}
	}

	//测试用 ./a.out > tmp  1秒立即中段
	//查看子进程结束状态
	while (1) {
		//tmp_pid = waitpid(-1, &status, WNOHANG);
		tmp_pid = wait(&status);
		printf("wait pid is:%d \n", (int)tmp_pid);
		if (WIFEXITED(status)) {
			printf("exit status %d\n", WEXITSTATUS(status));
		}
	}

	printf("parent finished\n");
	return 0;
}
