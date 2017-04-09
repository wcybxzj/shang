#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "signal_functions.h"

void func(int s)
{
	printf("%d\n",s);
}

int main(int argc, const char *argv[])
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	//阻塞SIGINT
	sigprocmask(SIG_SETMASK, &set, NULL);

	//阻塞SIGINT效果同上
	//sigprocmask(SIG_BLOCK, &set, NULL);

	//查看当前mask位
	printSigMask(stdout, "");


	while (1) {
		pause();
	}

	return 0;
}
