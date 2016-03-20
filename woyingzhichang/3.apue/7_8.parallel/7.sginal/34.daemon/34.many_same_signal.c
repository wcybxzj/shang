#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//一直ctrl+c 发送大量同一个信号
//信号会丢失
//同一个信号的处理是串行的

/*
./a.out 
^Csignal is 2
^C^C^C^C^C^C^C^C^C^Csignal is 2
^\退出 (核心已转储)
*/

void sig_handler(int s){
	printf("signal is %d\n", s);
	sleep(3);
}

int main(int argc, const char *argv[])
{

	signal(SIGINT, sig_handler);

	while (1) {
		pause();
	}

	return 0;
}
