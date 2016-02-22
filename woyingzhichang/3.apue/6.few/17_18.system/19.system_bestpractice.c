#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//调用system前先阻塞SIGCHLD,忽略SIGINT,SIGQUIT
int main(int argc, const char *argv[])
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, &oset);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	//system("date +%s");
	system("sleep 10");

	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	sigprocmask(SIG_SETMASK, &oset, NULL);
	return 0;
}
