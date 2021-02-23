#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <sys/types.h>

sigset_t newmask, oldmask;

void sig_usr1(int signo)
{
	fprintf(stdout, "caught SIGUSR1\n");
	return;
}

void sig_usr2(int signo)
{
	fprintf(stdout, "caught SIGUSR2\n");
	//恢复信号处理
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
	return;
}

//kill -USR1 pid  不会进行相应,但是USR1信号的发送被记录下来了
//kill -USR2 pid  USR2的信号处理 接触USR1的阻塞 USR1立刻进行信号处理
int main(void){
	printf( "Process id: %d\n", getpid() );
	// block SIGUSR1
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		perror("sigprocmask error");
	}

	signal(SIGUSR1, sig_usr1);
	signal(SIGUSR2, sig_usr2);
	while (1) {
		sleep(100);
	}
	return 0;
}
