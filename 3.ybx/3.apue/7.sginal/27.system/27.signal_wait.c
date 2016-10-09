#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

void sig_handler(int s);
void only_signal();
void signal_and_waitpid();

int main(int argc, const char *argv[])
{
	//only_signal();
	signal_and_waitpid();
	return 0;
}

void sig_handler(int s)
{
	int status;
	while(wait(&status) == -1) {
		if (errno==EINTR) {
			continue;
		}
		if (errno) {
			perror("sginal wait():");
			return;
		}
	}

	if (WIFEXITED(status)) {
		printf("signal wait status:%ld\n", WEXITSTATUS(status));
	}
}

void only_signal()
{
	signal(SIGCHLD, sig_handler);
	pid_t pid;
	int status;
	pid = fork();
	if (pid==0) {
		sleep(1);
		exit(11);
	}
	pause();
}

void signal_and_waitpid()
{
	signal(SIGCHLD, sig_handler);
	pid_t pid;
	int status;
	pid = fork();
	if (pid==0) {
		//sleep(1);
		execl("/bin/sh", "sh", "-c", "sleep 1", NULL);
		exit(11);
	}

	//注释pause
	//模拟的是waitpid和sig_handler竞争中waitpid成功
	//信号处理先报错:No child processes，然后的wait/waitpid正常回收状态

	//开启pause
	//模拟的是waitpid和sig_handler竞争中sig_handler成功
	//信号先响应回收子进程状态,最后的wait/waitpid报错No child processes

	//pause();

	while(waitpid(pid, &status, 0) == -1) {
	//while(wait(&status) == -1) {
		if (errno==EINTR) {
			continue;
		}
		if (errno) {
			perror("my_system waitpid():");
			exit(1);
		}
	}
	if (WIFEXITED(status)) {
		printf("normal wait status:%ld\n", WEXITSTATUS(status));
	}
}
