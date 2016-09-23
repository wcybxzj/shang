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

void handler(int signum){
	printf("tid:%d handler work!\n", gettid());
	pthread_exit(NULL);
}

void *thread(void *arg) {
	int i;
	pthread_t main_tid = (pthread_t) arg;
	printf("child tid:%d\n", gettid());
	sigset_t set;

	signal(SIGTERM, handler);

	for (i = 0; i < 100; i++) {
		pthread_kill(main_tid, SIGUSR2);
		sleep(1);
	}
	return NULL;
//main线程 kill其他线程的2个方法
//方法1:
//pthread_kill(tid, SIGTERM);
//必须设置信号处理程序最后调用pthread_exit

//方法2:pthread_cancel(tid);
int main(int argc, char *argv[])
{
	pthread_t tid;
	pthread_t main_tid;
	sigset_t set;
	int sig, ret;
	int num = 0;

	printf("main tid:%d \n", gettid());

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);

	if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
		perror("pthread_sigmask");
		pthread_exit(NULL);
	}

	main_tid = pthread_self();

	if (pthread_create(&tid, NULL, thread, (void *) main_tid) != 0) {
		perror("pthread_create");
		exit(1);
	}

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	while (1) {
		printf("block\n");
		ret = sigwait(&set, &sig);
		if (ret) {
			perror("sigwait()");
			exit(-2);
		}
		printf("unblock\n");

		if(num == 5){
			pthread_kill(tid, SIGTERM);
			//pthread_cancel(tid);
		}
		num++;
	}

	pthread_exit(NULL);
}
