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

static void handler(int signum) {
	printf("tid:%d handler work!\n", gettid());
}

void *thread(void *arg) {
	int i;
	pthread_t main_tid = (pthread_t) arg;
	printf("child tid:%d\n", gettid());
	for (i = 0; i < 1000; i++) {
		pthread_kill(main_tid, SIGUSR2);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t tid;
	pthread_t main_tid;
	sigset_t set, oset;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	if (pthread_sigmask(SIG_BLOCK, &set, &oset) != 0) {
		perror("pthread_sigmask");
		pthread_exit(NULL);
	}

	signal(SIGUSR2, handler);
	main_tid = pthread_self();

	if (pthread_create(&tid, NULL, thread, (void *) main_tid) != 0) {
		perror("pthread_create");
		exit(1);
	}

	printf("main tid:%d \n", gettid());
	while (1) {
		printf("block\n");
		sigsuspend(&oset);
		printf("unblock\n");
	}

	pthread_exit(NULL);
}
