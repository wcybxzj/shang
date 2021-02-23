#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <sys/types.h>
#include <unistd.h>


void sig_usr1(int signo)
{
	fprintf(stdout, "caught SIGUSR1\n");
	return;
}

void sig_usr2(int signo)
{
	fprintf(stdout, "caught SIGUSR2\n");
	signal(SIGUSR1, sig_usr1);
	return;
}

//kill -USR1 pid  不会进行相应
//kill -USR2 pid  USR2的信号处理 虽然USR1的处理打开了 但是之前的USR1的信号不会被
//kill -USR1 pid  USR1正常处理
int main(void){

	printf( "Process id: %d\n", getpid() );

	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, sig_usr2);

	while(1){
		sleep(1000);
	}
	return 0;
}
