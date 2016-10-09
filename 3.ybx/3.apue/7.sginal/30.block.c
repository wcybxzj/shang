#include <stdio.h>
#include <stdlib.h>
Finclude <signal.h>
void int_handler(int s){
	write(1, "!",1);
}


//1.在内部循环中不响应信号，在外部循环中响应
//2.玩命Ctrl+C，也只能响应一次,并且必须是打印完5个星号的时候.因为信号pending位是位图只能保存一次信号
//3.模块化思维
void star()
{
	sigset_t set, oset, saveset;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_UNBLOCK, &set, &saveset);//模块化思维,备份当前所有信号屏蔽字
	int i, j;
	signal(SIGINT, int_handler);
	for (j = 0; j < 1000; j++) {
		sigprocmask(SIG_BLOCK, &set, &oset);
		for (i = 0; i < 5; i++) {
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);
		sigprocmask(SIG_SETMASK, &oset, NULL);
	}
	sigprocmask(SIG_SETMASK, &saveset, NULL);//模块化思维,复原进入模块时候的信号屏蔽字
}

int main(int argc, const char *argv[])
{
	star();
	return 0;
}
