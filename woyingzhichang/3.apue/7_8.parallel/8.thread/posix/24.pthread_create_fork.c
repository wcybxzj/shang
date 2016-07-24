#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

//child thread: pid:11914 ,tid:11915
//child thread: parent pid:11914, tid:11915
//child thread: parent pid:11916, tid:11916
//main thread: pid:11914
//main thread: tid:11914
pid_t gettid(void)
{
  return syscall(SYS_gettid);
}

static void* func(void *ptr){
	pid_t tid;
	tid = gettid();
	printf("child thread: pid:%d ,tid:%d\n", getpid(), tid);
	fflush(NULL);

	pid_t pid;
	pid = fork();
	if(pid > 0){
		printf("child thread: parent pid:%d, tid:%d\n", getpid(),gettid());
	}else{               
		printf("child thread: parent pid:%d, tid:%d\n", getpid(),gettid());
	}

	//sleep(100);
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
