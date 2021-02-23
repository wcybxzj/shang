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
测试1:不同信号嵌套

./40.star
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
测试2:相同信号不嵌套

./40.star
pid:11091
^C信号:2 ,触发时间:1614103605,i = 0
信号:2 ,触发时间:1614103605,i = 1
^C信号:2 ,触发时间:1614103605,i = 2
信号:2 ,触发时间:1614103605,i = 3
信号:2 ,触发时间:1614103605,i = 4
信号:2 ,触发时间:1614103615,i = 0
信号:2 ,触发时间:1614103615,i = 1
信号:2 ,触发时间:1614103615,i = 2
信号:2 ,触发时间:1614103615,i = 3
信号:2 ,触发时间:1614103615,i = 4
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
