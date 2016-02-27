#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void sig_handler(int s);
int system_ok(char *cmd);
int system_bad(char *cmd);

//分析为什么要在system阻塞SIGCHLD,执行完后解除此阻塞
int main(int argc, const char *argv[])
{
	int ret, i;
	signal(SIGCHLD, sig_handler);
	pid_t pid;
	pid = fork();
	if (pid==0) {
		sleep(3);
		exit(11);
	}
	if (1) {
		//bad
		ret = system_bad("sleep 1");
		if (ret) {
			printf("system ret is fail: %s\n", strerror(-ret));
		}else{
			printf("system ret is success\n");
		}
		pause();
	}else{
		//ok
		ret = system_ok("sleep 1");
		if (ret) {
			printf("system ret is fail: %s\n", strerror(-ret));
		}else{
			printf("system ret is success\n");
		}
	}
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

int system_bad(char *cmd)
{
	int i;
	int status;
	pid_t pid;

	pid = fork();
	if (pid==0) {
		execl("/bin/sh", "sh", "-c", cmd, NULL);
		perror("execl");
		return -1;
	}

	//必须用sleep而不是pause
	//因为目的是模拟system子进程结束,但waitpid还没执行,让信号处理回收了system的子进程
	sleep(2);

	//一直在等待
	while(waitpid(pid, &status, 0) == -1) {
		if (errno==EINTR) {
			continue;
		}
		if (errno) {
			return -errno;
		}
	}

	if (WIFEXITED(status)) {
		//printf("my_system waitpid status:%ld\n", WEXITSTATUS(status));
		return WEXITSTATUS(status);
	}
	return -3;
}

int system_ok(char *cmd)
{
	int status;
	pid_t pid;

	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, &oset);

	pid = fork();
	if (pid==0) {
		execl("/bin/sh", "sh", "-c", cmd, NULL);
		perror("execl");
		return -1;
	}

	//必须用sleep而不是pause
	//因为目的是模拟system子进程结束,但waitpid还没执行,让信号处理回收了system的子进程
	sleep(2);

	//一直在等待
	while(waitpid(pid, &status, 0) == -1) {
		if (errno==EINTR) {
			continue;
		}
		if (errno) {
			return -errno;
		}
	}

	sigprocmask(SIG_SETMASK, &oset, NULL);
	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	}
	return -3;
}
