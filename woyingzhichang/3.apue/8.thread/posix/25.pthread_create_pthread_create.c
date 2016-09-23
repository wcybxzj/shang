#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
//目的:pthread_create中再次pthread_create
pid_t gettid(void)
{
  return syscall(SYS_gettid);
}

static void* func1(void *ptr){
	int i;
	for(i=0; i<10;i++){
		printf("func1:%d\n", i);
		sleep(1);
	}
	pthread_exit(NULL);
}

static void* func(void *ptr){
	int i;
	pthread_t tid;
	pthread_create(&tid, NULL, func1, NULL);
	for(i=0; i<10;i++){
		printf("func:%d\n", i);
		sleep(1);
	}
	pthread_exit(NULL);
}

int main(void){
	int err;
	pthread_t tid;
	err = pthread_create(&tid, NULL, func, NULL);
	pthread_join(tid, NULL);
	
	pid_t main_tid;
	main_tid = gettid();
	printf("main thread: pid:%d\n", getpid());
	printf("main thread: tid:%d\n", main_tid);
	fflush(NULL);

	pid_t pid;
	pid = fork();

	return 0;
}
