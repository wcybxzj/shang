#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>



void show_handler(int sig)
{
	time_t t;
	time(&t);
	int i;
	for(i = 0; i < 5; i++) {
		printf("信号:%d ,触发时间:%ld,i = %d\n", sig, t, i);
		sleep(2);
	}
}

/*
测试1:
发送完ctrl+c 还没处理完又发送ctrl+\
ctrl+c的处理程序被ctrl+\的处理打断

[root@web11 40.SA_NODEFER_SA_RESETHAND]# ./40.star
pid:15175
^C信号:2 ,触发时间:1518716181,i = 0
信号:2 ,触发时间:1518716181,i = 1
^\信号:3 ,触发时间:1518716185,i = 0
信号:3 ,触发时间:1518716185,i = 1
信号:3 ,触发时间:1518716185,i = 2
信号:3 ,触发时间:1518716185,i = 3
信号:3 ,触发时间:1518716185,i = 4
信号:2 ,触发时间:1518716181,i = 2
信号:2 ,触发时间:1518716181,i = 3
信号:2 ,触发时间:1518716181,i = 4
Terminated
*/

/*
测试2:连续发送两个相同信号，发现被依次处理没出现想测试1被打断的情况
[root@web11 40.SA_NODEFER_SA_RESETHAND]# ./40.star
pid:15213
^C信号:2 ,触发时间:1518716409,i = 0
^C信号:2 ,触发时间:1518716409,i = 1
信号:2 ,触发时间:1518716409,i = 2
信号:2 ,触发时间:1518716409,i = 3
信号:2 ,触发时间:1518716409,i = 4
信号:2 ,触发时间:1518716419,i = 0
信号:2 ,触发时间:1518716419,i = 1
信号:2 ,触发时间:1518716419,i = 2
信号:2 ,触发时间:1518716419,i = 3
信号:2 ,触发时间:1518716419,i = 4
Terminated
*/
int main(void)
{
	printf("pid:%d\n",getpid());

	struct sigaction act, oldact;
	act.sa_handler = show_handler;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGINT, &act, &oldact);
	sigaction(SIGQUIT, &act, &oldact);

	while(1) {
		pause();
	}
}
