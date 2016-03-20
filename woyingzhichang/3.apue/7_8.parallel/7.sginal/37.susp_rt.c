#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//可以看到实时信号不丢失,排队执行
//ulimit -a中pending signals是表示实时信号排队的允许长度
#define SIG_RT35 SIGRTMIN+1

void int_handler(int s){
	write(1, "!",1);
}

void star()
{
	sigset_t set, oset, saveset,tmpset;
	sigemptyset(&set);
	sigaddset(&set, SIG_RT35);
	sigprocmask(SIG_UNBLOCK, &set, &saveset);//模块化思维,备份当前所有信号屏蔽字
	int i, j;
	signal(SIG_RT35, int_handler);
	sigprocmask(SIG_BLOCK, &set, &oset);
	for (j = 0; j < 1000; j++) {
		for (i = 0; i < 5; i++) {
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);

		sigsuspend(&oset);
	}
	sigprocmask(SIG_SETMASK, &saveset, NULL);//模块化思维,复原进入模块时候的信号屏蔽字
}

int main(int argc, const char *argv[])
{
	star();
	return 0;
}
