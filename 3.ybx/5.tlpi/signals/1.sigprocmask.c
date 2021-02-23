#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "signal_functions.h"


/*输出:
empty signal set>
2 (Interrupt)
empty signal set>
2 (Interrupt)
*/



int main(int argc, const char *argv[])
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	//print
	printSigMask(stdout,NULL);
	//阻塞SIGINT
	sigprocmask(SIG_SETMASK, &set, &oset);
	//print
	printSigMask(stdout,NULL);

	//恢复
	sigprocmask(SIG_SETMASK, &oset, NULL);
	//print
	printSigMask(stdout,NULL);
	//阻塞SIGINT效果同上
	sigprocmask(SIG_BLOCK, &set, NULL);
	//print
	printSigMask(stdout,NULL);


	while (1) {
		pause();
	}

	return 0;
}
