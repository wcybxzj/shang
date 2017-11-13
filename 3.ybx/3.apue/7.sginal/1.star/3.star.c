#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void int_handler(int s){
	write(1, "!",1);
}

//目的:想用SA_RESTART来让被ctrl+c打断的sleep复位正常工作
//结果:失败
//原因:sleep 不是系统调用(man 3 sleep)
int main(int argc, const char *argv[])
{
	int i;
	struct sigaction act;
	struct sigaction oldact;

	act.sa_handler = int_handler;
	act.sa_flags = 0;
	act.sa_flags |= SA_RESTART;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGINT, &act, &oldact) == -1) {
		printf("sigaction()\n");
		exit(1);
	}

	//signal(SIGINT, SIG_IGN);//忽略此信号
	//signal(SIGINT, int_handler);

	for (i = 0; i < 10; i++) {
		write(1, "*", 1);
		sleep(1);
	}
	return 0;
}
