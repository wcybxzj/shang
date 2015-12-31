#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void int_handler(int sig, siginfo_t * siginfo ,void *upc){
	//没用因为SIGINT 不能被忽略
	if (siginfo->si_code != SI_KERNEL ) {
		return;
	}
	write(1, "!", 1);
}

/*
 * 需求:输出5个星号中无论多少个ctrl+c不会打断sleep,但是输出后会输出!,
 * 然后继续输出5个星号.
 *
 * pause:信号设置在5个星号中,一解除阻塞立刻就会执行，所以会停在pause()
 *
 * sigsuspend:就是用sigsuspend() 很好的体现了它的原子性,  解除阻塞,触发信号,回复阻塞是原子性的
 */

int main(int argc, const char *argv[])
{
	int i, j, ret;
	sigset_t set, oset, saveset; 
	struct sigaction sa, osa;
	sa.sa_sigaction = int_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;


	if (sigaction(SIGINT, &sa, &osa) < 0) {
		perror("sigaction()");
		exit(1);
	}

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	
	sigprocmask(SIG_UNBLOCK, &set, &saveset);
	sigprocmask(SIG_BLOCK, &set, &oset);
	for (i = 0; i < 10000; i++) {
		for (j = 0; j < 5; j++) {
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);

		sigsuspend(&oset);

		//sigset_t tmpset;
		//sigprocmask(SIG_SETMASK, &oset, &tmpset);
		//pause();
		//sigprocmask(SIG_SETMASK, &tmpset, NULL);
	}
	sigprocmask(SIG_SETMASK, &saveset, NULL);
	return 0;
}
