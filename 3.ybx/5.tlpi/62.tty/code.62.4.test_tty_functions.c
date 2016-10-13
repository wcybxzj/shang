#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

void errExit(char *str)
{
	perror(str);
	exit(1);
}

static void sig_hander2(int s){
	printf("2222\n");
}

static void             /* Handler for SIGTSTP */
tstpHandler(int sig)
{
    struct termios ourTermios;          /* To save our tty settings */
    sigset_t tstpMask, prevMask;
    struct sigaction sa;
    int savedErrno;

    //if (signal(SIGTSTP, sig_hander2) == SIG_ERR)
    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR)
	{
		perror("signal");
		exit(1);
	}
    raise(SIGTSTP);

	int i;
	for (i = 0; i < 5; i++) {
		printf("sleep 111111111111\n");
		sleep(1);
	}

    sigemptyset(&tstpMask);
    sigaddset(&tstpMask, SIGTSTP);
    if (sigprocmask(SIG_UNBLOCK, &tstpMask, &prevMask) == -1){
        perror("sigprocmask");
		exit(1);
	}

	for (i = 0; i < 5; i++) {
		printf("sleep 22222222222\n");
		sleep(1);
	}

    ///* Execution resumes here after SIGCONT */
    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask");         /* Reblock SIGTSTP */

    sigemptyset(&sa.sa_mask);           /* Reestablish handler */
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstpHandler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1)
        errExit("sigaction");

    /* The user may have changed the terminal settings while we were
       stopped; save the settings so we can restore them later */

    //if (tcgetattr(STDIN_FILENO, &userTermios) == -1)
    //    errExit("tcgetattr");

    ///* Restore our terminal settings */

    //if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ourTermios) == -1)
    //    errExit("tcsetattr");

    errno = savedErrno;
}

int main(int argc, const char *argv[])
{
	printf("pid:%d\n", getpid());
	int n;
	char ch;
	struct sigaction sa, prev;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

	sa.sa_handler = tstpHandler;

	if (sigaction(SIGTSTP, NULL, &prev) == -1){
		perror("sigaction");
		exit(1);
	}

	if (prev.sa_handler != SIG_IGN){
		if (sigaction(SIGTSTP, &sa, NULL) == -1){
			perror("sigaction");
			exit(1);
		}
	}


	while (1) {
		n  = read(STDIN_FILENO, &ch,1);
		printf("n is %d\n",n);
		if (n == -1) {
			perror("read()");
			break;
		}
		if (n==0) {
			break;
		}

		putchar(ch);


	}

	while (1) {
		pause();
	}

	return 0;
}
