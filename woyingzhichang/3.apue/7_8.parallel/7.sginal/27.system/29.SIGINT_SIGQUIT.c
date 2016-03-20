#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int system_ok(char *cmd);

int main(int argc, const char *argv[])
{
	int ret;
	ret = system_ok("sleep 20");
	printf("system ret is %d\n", ret);

	return 0;
}

int system_ok(char *cmd)
{
	int status;
	pid_t pid;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, &oset);

	pid = fork();
	if (pid==0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execl("/bin/sh", "sh", "-c", cmd, NULL);
		perror("execl");
		_exit(127);
	}

	while(waitpid(pid, &status, 0) == -1) {
		if (errno==EINTR) {
			continue;
		}
		if (errno) {
			return -2;
		}
	}
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	sigprocmask(SIG_SETMASK, &oset, NULL);
	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	}
	if (WIFSIGNALED(status)) {
		printf("killed by signal %d\n", WTERMSIG(status));
	}
	return -3;//进程异常退出
}
