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
测试1:不同信号嵌套执行
./41.star_SA_NODEFER
pid:11088
^C信号:2 ,触发时间:1614103391,i = 0
信号:2 ,触发时间:1614103391,i = 1
^\信号:3 ,触发时间:1614103393,i = 0
信号:3 ,触发时间:1614103393,i = 1
信号:3 ,触发时间:1614103393,i = 2
信号:3 ,触发时间:1614103393,i = 3
信号:3 ,触发时间:1614103393,i = 4
信号:2 ,触发时间:1614103391,i = 2
信号:2 ,触发时间:1614103391,i = 3
信号:2 ,触发时间:1614103391,i = 4
Terminated

*/



/*
测试2:相同信号嵌套
./41.star_SA_NODEFER 
pid:11329
^C信号:2 ,触发时间:1614104639,i = 0
信号:2 ,触发时间:1614104639,i = 1
^C信号:2 ,触发时间:1614104642,i = 0
信号:2 ,触发时间:1614104642,i = 1
信号:2 ,触发时间:1614104642,i = 2
信号:2 ,触发时间:1614104642,i = 3
信号:2 ,触发时间:1614104642,i = 4
信号:2 ,触发时间:1614104639,i = 2
信号:2 ,触发时间:1614104639,i = 3
信号:2 ,触发时间:1614104639,i = 4
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
