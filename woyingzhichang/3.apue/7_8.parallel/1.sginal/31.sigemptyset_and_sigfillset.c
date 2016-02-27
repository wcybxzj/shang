#include <stdio.h>
#include <signal.h>

#define BUF_SIZE 100

char *
ALT_strsignal(int sig)
{
    static char buf[BUF_SIZE];          /* Not thread-safe */

    snprintf(buf, BUF_SIZE, "SIG-%d", sig);
    return buf;
}

void                    /* Print list of signals within a signal set */
printSigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig, cnt;

    cnt = 0;
    for (sig = 1; sig < 64; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, ALT_strsignal(sig));
        }
    }

    if (cnt == 0)
        fprintf(of, "%s<empty signal set>\n", prefix);
}

void sigemptyset_and_sigfillset()
{
	sigset_t set;
	sigemptyset(&set);
	printSigset(stdout, "\t\t", &set);
	printf("---------------------\n");
	sigfillset(&set);
	printSigset(stdout, "\t\t", &set);
}

int main(int argc, const char *argv[])
{
	sigemptyset_and_sigfillset();
	return 0;
}
