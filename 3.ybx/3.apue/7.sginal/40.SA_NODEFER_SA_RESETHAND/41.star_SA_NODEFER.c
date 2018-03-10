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
连续发送2个信号,通过SA_NODEFER参数
让相同信号的处理不是排队,而是打断

[root@web11 40.SA_NODEFER_SA_RESETHAND]# ./41.star
pid:15256
^C信号:2 ,触发时间:1518716568,i = 0
^C信号:2 ,触发时间:1518716570,i = 0
信号:2 ,触发时间:1518716570,i = 1
信号:2 ,触发时间:1518716570,i = 2
信号:2 ,触发时间:1518716570,i = 3
信号:2 ,触发时间:1518716570,i = 4
信号:2 ,触发时间:1518716568,i = 1
信号:2 ,触发时间:1518716568,i = 2
信号:2 ,触发时间:1518716568,i = 3
信号:2 ,触发时间:1518716568,i = 4
Terminated
*/
int main(void)
{
	printf("pid:%d\n",getpid());

	struct sigaction act, oldact;
	act.sa_handler = show_handler;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags = SA_NODEFER;//别名SA_NOMASK

	sigaction(SIGINT, &act, &oldact);
	sigaction(SIGQUIT, &act, &oldact);

	while(1) {
		pause();
	}
}
