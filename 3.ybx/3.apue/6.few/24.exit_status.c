#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int i, status;
	pid_t pid, tmp_pid;
	pid = fork();
	if (pid < 0) {
		exit(1);
	}

	if (pid == 0) {
		printf("child is %d\n", getpid());
		sleep(1000);
		exit(123);
	}

	tmp_pid = wait(&status);
	printf("wait pid is:%d \n", (int)tmp_pid);
	if (WIFEXITED(status)) {
		printf("进程正常终止\n");
		printf("子进程 退出状态:%d\n", WEXITSTATUS(status));
	}else {
		printf("进程异常终止\n");
	}
	if (WIFSIGNALED(status)) {
		printf("子进程 被信号杀死,信号是:%d\n",WTERMSIG(status));
	}
	if (WIFSTOPPED(status)) {
		printf("子进程 引起停止的信号:%d\n", WSTOPSIG(status));
	}
	if (WIFCONTINUED(status)) {
		printf("子进程 重新运行\n");
	}

	printf("parent finished\n");
	return 0;
}
