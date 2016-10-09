#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/syscall.h>

pthread_t gettid() {
     return syscall(SYS_gettid);
}

void *thread(void *arg) {
	int i;
	pthread_t main_tid = (pthread_t) arg;
	printf("child tid:%d\n", gettid());
	for (i = 0; i < 100; i++) {
		pthread_kill(main_tid, SIGUSR2);
		sleep(1);
	}
	return NULL;
}

//sigwait比sigususpend方便 不需要对信号做sigaction处理
int main(int argc, char *argv[])
{
	pthread_t tid;
	pthread_t main_tid;
	sigset_t set;
	int sig, ret;

	printf("main tid:%d \n", gettid());

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	sigaddset(&set, SIGTERM);

	if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
		perror("pthread_sigmask");
		pthread_exit(NULL);
	}

	main_tid = pthread_self();

	if (pthread_create(&tid, NULL, thread, (void *) main_tid) != 0) {
		perror("pthread_create");
		exit(1);
	}

	while (1) {
		printf("block\n");
		ret = sigwait(&set, &sig);
		if (ret) {
			perror("sigwait()");
			exit(-2);
		}
		printf("unblock\n");
	}

	pthread_exit(NULL);
}
