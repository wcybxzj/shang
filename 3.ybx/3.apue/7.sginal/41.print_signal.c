#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>



//使用信号做一个计时器
//  1) SIGHUP	 2) SIGINT	 3) SIGQUIT	 4) SIGILL	 5) SIGTRAP
//  6) SIGABRT	 7) SIGBUS	 8) SIGFPE	 9) SIGKILL	10) SIGUSR1
// 11) SIGSEGV	12) SIGUSR2	13) SIGPIPE	14) SIGALRM	15) SIGTERM
// 16) SIGSTKFLT	17) SIGCHLD	18) SIGCONT	19) SIGSTOP	20) SIGTSTP
// 21) SIGTTIN	22) SIGTTOU	23) SIGURG	24) SIGXCPU	25) SIGXFSZ
// 26) SIGVTALRM	27) SIGPROF	28) SIGWINCH	29) SIGIO	30) SIGPWR
// 31) SIGSYS	34) SIGRTMIN	35) SIGRTMIN+1	36) SIGRTMIN+2	37) SIGRTMIN+3
// 38) SIGRTMIN+4	39) SIGRTMIN+5	40) SIGRTMIN+6	41) SIGRTMIN+7	42) SIGRTMIN+8
// 43) SIGRTMIN+9	44) SIGRTMIN+10	45) SIGRTMIN+11	46) SIGRTMIN+12	47) SIGRTMIN+13
// 48) SIGRTMIN+14	49) SIGRTMIN+15	50) SIGRTMAX-14	51) SIGRTMAX-13	52) SIGRTMAX-12
// 53) SIGRTMAX-11	54) SIGRTMAX-10	55) SIGRTMAX-9	56) SIGRTMAX-8	57) SIGRTMAX-7
// 58) SIGRTMAX-6	59) SIGRTMAX-5	60) SIGRTMAX-4	61) SIGRTMAX-3	62) SIGRTMAX-2
// 63) SIGRTMAX-1	64) SIGRTMAX
static void f1(int s)
{
	int i;
	printf("start signal:%d\n", s);
}

//目的:想测一下pkill是如何发信号的
//pkill -(要发的信号) 指定的进程名称
//pkill -1 a.out
//pkill -2 a.out
int main()
{
	printf("pid:%d\n",getpid());

	int i;
	struct sigaction sa;
	sa.sa_handler = f1;
	sigemptyset(&sa.sa_mask);

	sigaddset(&sa.sa_mask,SIGHUP);
	sigaddset(&sa.sa_mask,SIGABRT);
	sigaddset(&sa.sa_mask,SIGSEGV);
	sigaddset(&sa.sa_mask,SIGSTKFLT);
	sigaddset(&sa.sa_mask,SIGTTIN);
	sigaddset(&sa.sa_mask,SIGVTALRM);
	sigaddset(&sa.sa_mask,SIGSYS);
	sigaddset(&sa.sa_mask,SIGINT);
	sigaddset(&sa.sa_mask,SIGBUS);
	sigaddset(&sa.sa_mask,SIGUSR2);
	sigaddset(&sa.sa_mask,SIGCHLD);
	sigaddset(&sa.sa_mask,SIGTTOU);
	sigaddset(&sa.sa_mask,SIGPROF);
	sigaddset(&sa.sa_mask,SIGRTMIN);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sigaddset(&sa.sa_mask,SIGFPE);
	sigaddset(&sa.sa_mask,SIGPIPE);
	sigaddset(&sa.sa_mask,SIGCONT);
	sigaddset(&sa.sa_mask,SIGURG);
	sigaddset(&sa.sa_mask,SIGWINCH);
	sigaddset(&sa.sa_mask,SIGILL);
	sigaddset(&sa.sa_mask,SIGKILL);
	sigaddset(&sa.sa_mask,SIGALRM);
	sigaddset(&sa.sa_mask,SIGSTOP);
	sigaddset(&sa.sa_mask,SIGXCPU);
	sigaddset(&sa.sa_mask,SIGIO);
	sigaddset(&sa.sa_mask,SIGTRAP);
	sigaddset(&sa.sa_mask,SIGUSR1);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGTSTP);
	sigaddset(&sa.sa_mask,SIGXFSZ);
	sigaddset(&sa.sa_mask,SIGPWR);

	sa.sa_flags = 0;

	sigaction(SIGHUP        ,&sa,NULL);
	sigaction(SIGABRT       ,&sa,NULL);
	sigaction(SIGSEGV       ,&sa,NULL);
	sigaction(SIGSTKFLT     ,&sa,NULL);
	sigaction(SIGTTIN       ,&sa,NULL);
	sigaction(SIGVTALRM     ,&sa,NULL);
	sigaction(SIGSYS        ,&sa,NULL);
	sigaction(SIGINT        ,&sa,NULL);
	sigaction(SIGBUS        ,&sa,NULL);
	sigaction(SIGUSR2       ,&sa,NULL);
	sigaction(SIGCHLD       ,&sa,NULL);
	sigaction(SIGTTOU       ,&sa,NULL);
	sigaction(SIGPROF       ,&sa,NULL);
	sigaction(SIGRTMIN      ,&sa,NULL);
	sigaction(SIGQUIT       ,&sa,NULL);
	sigaction(SIGFPE        ,&sa,NULL);
	sigaction(SIGPIPE       ,&sa,NULL);
	sigaction(SIGCONT       ,&sa,NULL);
	sigaction(SIGURG        ,&sa,NULL);
	sigaction(SIGWINCH      ,&sa,NULL);
	sigaction(SIGILL        ,&sa,NULL);
	sigaction(SIGKILL       ,&sa,NULL);
	sigaction(SIGALRM       ,&sa,NULL);
	sigaction(SIGSTOP       ,&sa,NULL);
	sigaction(SIGXCPU       ,&sa,NULL);
	sigaction(SIGIO         ,&sa,NULL);
	sigaction(SIGTRAP       ,&sa,NULL);
	sigaction(SIGUSR1       ,&sa,NULL);
	sigaction(SIGTERM       ,&sa,NULL);
	sigaction(SIGTSTP       ,&sa,NULL);
	sigaction(SIGXFSZ       ,&sa,NULL);
	sigaction(SIGPWR        ,&sa,NULL);

	while (1) {
		pause();
	}

	exit(0);
}


