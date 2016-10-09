#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/syscall.h>

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

pid_t gettid()
{
    return syscall(SYS_gettid);
}

void sig_handler(int s){
	printf("tid:%d, sig_handler work!\n", pthread_self());
}

void sig_block(){
	int s;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	//用在main thread效果一样
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
	//s = sigprocmask(SIG_BLOCK, &set, NULL);
	if (s != 0){
		handle_error_en(s, "pthread_sigmask");
	}
}

void sig_unblock(){
	int s;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	s = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	if (s != 0){
		handle_error_en(s, "pthread_sigmask");
	}
}

static void * sig_thread(void *arg)
{
	int i;

	printf("child thread:%d\n", gettid());
	for (i = 0; i < 3; i++) {
		printf("child thread up sleep 1 \n");
		sleep(1);
	}

	sig_unblock();

	for (i = 0; i < 3; i++) {
		printf("child thread  down sleep 1 \n");
		sleep(1);
	}

	pthread_exit(NULL);
}

//证明1:子线程继承main线程的信号处理方式
//证明2:子线程继承main线程信号掩码
//main  thread 使用 sig_block
//child thread 使用 sig_unblock
//子线程必须unblock进程来的信号掩码才能用继承来的信号处理函数响应信号

int main(int argc, char *argv[])
{
	pthread_t tid;
	int s;
	struct sigaction sa;

	printf("main thread:%d\n", gettid());

	sig_block();

	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sa.sa_flags = 0;
	sigaction(SIGUSR2, &sa, NULL);

	s = pthread_create(&tid, NULL, &sig_thread, NULL);
	if (s != 0){
		handle_error_en(s, "pthread_create");
	}

	sleep(1);
	printf("main thread send signal!\n");
	pthread_kill(tid, SIGUSR2);

	pause();
}

