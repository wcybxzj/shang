#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

static sigset_t oset;

void sig_handler(int s)
{
	int status;
	wait(&status);
	if (WIFEXITED(status)) {
		printf("first wait status:%ld\n", WEXITSTATUS(status));
	}else{
		printf("abnormal\n");
	}
}

void my_system()
{
	int status;
	pid_t pid;
	pid = fork();
	if (pid < 0 ) {
		perror("fork():");
		exit(1);
	}
	if (pid == 0) {
		//execl("/bin/sh", "sh", "-c", "date +%s", NULL);
		execl("/bin/sh", "sh", "-c", "sleep 3", NULL);
		perror("execl");
		exit(1);
	}
	waitpid(pid,&status,0);
	if (WIFEXITED(status)) {
		printf("first wait status:%ld\n", WEXITSTATUS(status));
	}else{
		printf("abnormal\n");
	}
	//sigsuspend(&oset);
}

void best_use_system()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, &oset);
	//signal(SIGQUIT, SIG_IGN);
	//signal(SIGINT, SIG_IGN);

	my_system();

	//signal(SIGQUIT, SIG_DFL);
	//signal(SIGINT, SIG_DFL);
	sigprocmask(SIG_SETMASK, &oset, NULL);
}

//模拟system的执行过程模拟
//调用system前先阻塞SIGCHLD,忽略SIGINT,SIGQUIT
//阻塞SIGCHLD保证system内部的wait接受自己子进程的状态改变
//忽略
int main(int argc, const char *argv[])
{
	//signal(SIGCHLD, sig_handler);

	int status;
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		printf("第一个子进程\n");
		exit(100);
	}
	best_use_system();
	wait(&status);
	if (WIFEXITED(status)) {
		printf("second wait status:%ld\n", WEXITSTATUS(status));
	}
	return 0;
}
