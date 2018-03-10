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
测试:连续发送ctrl+c,第一个ctrl+c的处理没有被打断
但是因为使用SA_RESETHAND,第二个ctrl+c已经变成默认行为程序被终止
[root@web11 40.SA_NODEFER_SA_RESETHAND]# ./42.star 
pid:15339
^C信号:2 ,触发时间:1518716901,i = 0
^C信号:2 ,触发时间:1518716901,i = 1
信号:2 ,触发时间:1518716901,i = 2
信号:2 ,触发时间:1518716901,i = 3
信号:2 ,触发时间:1518716901,i = 4

*/
int main(void)
{
	printf("pid:%d\n",getpid());

	struct sigaction act, oldact;
	act.sa_handler = show_handler;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags =SA_RESETHAND;//别名SA_ONESHOT

	sigaction(SIGINT, &act, &oldact);
	sigaction(SIGQUIT, &act, &oldact);

	while(1) {
		pause();
	}
}
