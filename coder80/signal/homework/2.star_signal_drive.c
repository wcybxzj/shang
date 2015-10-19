#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void int_handler(int sig, siginfo_t * siginfo ,void *upc){
	write(1, "!", 1);
}

int main(int argc, const char *argv[])
{
	int i, j, ret;
	sigset_t set, oset; 
	struct sigaction sa, osa;
	sa.sa_sigaction = int_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGINT, &sa, &osa) < 0) {
		perror("sigaction()");
		exit(1);
	}

	for (i = 0; i < 10000; i++) {
		for (j = 0; j < 5; j++) {
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);
		ret = pause();
	}
	return 0;
}
