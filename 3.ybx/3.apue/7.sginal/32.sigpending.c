#include <stdio.h>
#include <signal.h>

#define BUF_SIZE 100



void int_handler(int s);
char * ALT_strsignal(int sig);
void printSigset(FILE *of, const char *prefix, const sigset_t *sigset);
void understand_pause();
void understand_sigpending();

int main(int argc, const char *argv[])
{
	//understand_pause();

	understand_sigpending();

	//signal(SIGINT, int_handler);
	//pause();

	return 0;
}

void int_handler(int s)
{
	sigset_t pending;
	sigpending(&pending);
	printSigset(stdout, "\t\t", &pending);
}

char * ALT_strsignal(int sig)
{
    static char buf[BUF_SIZE];          /* Not thread-safe */

    snprintf(buf, BUF_SIZE, "SIG-%d", sig);
    return buf;
}

/* Print list of signals within a signal set */
void printSigset(FILE *of, const char *prefix, const sigset_t *sigset)
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

void understand_pause()
{
	sigset_t pending, set, oset;
	//阻塞所有能被阻塞的信号
	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, &oset);

	//因为信号被阻塞,没有任何信号能被处理所以这里一直阻塞, 除非kill -9 a.out
	//pause 让当前进程睡眠，直到有信号发送来并执行默认处理动作杀死进程或自定义信号处理函数才,puase才能返回
	pause();

	printf("if SIGINT,the line will never do!\n");
}

//3秒内ctrl+c
void understand_sigpending()
{
	sigset_t pending, pending2, set, oset;
	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, &oset);

	//必须用sleep 不能用pause
	sleep(3);

	sigpending(&pending);
	printSigset(stdout, "\t\t", &pending);

	sigpending(&pending2);
	printSigset(stdout, "\t\t", &pending2);

	sigprocmask(SIG_SETMASK, &oset, NULL);
}
