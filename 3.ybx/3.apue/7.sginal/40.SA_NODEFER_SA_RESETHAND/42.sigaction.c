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
测试1:不同信号不嵌套

*/



/*
测试2:相同信号不嵌套

*/

/*
root@ybx-virtual-machine:~/www/shang/3.ybx/3.apue/7.sginal/40.SA_NODEFER_SA_RESETHAND# ./42.sigaction 
pid:11354
^C信号:2 ,触发时间:1614105134,i = 0
信号:2 ,触发时间:1614105134,i = 1
^\信号:2 ,触发时间:1614105134,i = 2
信号:2 ,触发时间:1614105134,i = 3
信号:2 ,触发时间:1614105134,i = 4
信号:3 ,触发时间:1614105144,i = 0
信号:3 ,触发时间:1614105144,i = 1
信号:3 ,触发时间:1614105144,i = 2
信号:3 ,触发时间:1614105144,i = 3
信号:3 ,触发时间:1614105144,i = 4
Terminated

root@ybx-virtual-machine:~/www/shang/3.ybx/3.apue/7.sginal/40.SA_NODEFER_SA_RESETHAND# ./42.sigaction 
pid:11360
^C信号:2 ,触发时间:1614105178,i = 0
信号:2 ,触发时间:1614105178,i = 1
^C信号:2 ,触发时间:1614105178,i = 2
信号:2 ,触发时间:1614105178,i = 3
信号:2 ,触发时间:1614105178,i = 4
信号:2 ,触发时间:1614105188,i = 0
信号:2 ,触发时间:1614105188,i = 1
信号:2 ,触发时间:1614105188,i = 2
信号:2 ,触发时间:1614105188,i = 3
信号:2 ,触发时间:1614105188,i = 4
Terminated
*/


int main(void)
{
	printf("pid:%d\n",getpid());

	struct sigaction sa;

	sa.sa_handler = show_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGQUIT);
	sigaddset(&sa.sa_mask, SIGINT);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);


	while(1) {
		pause();
	}
}
