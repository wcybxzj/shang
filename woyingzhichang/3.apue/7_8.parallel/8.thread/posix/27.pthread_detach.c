#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

pid_t gettid()
{
     return syscall(SYS_gettid);
}

//测试1:
//main线程立刻结束不用for, child线程不结束使用for
//结果:
//进程成为僵尸因为子进程还要用进程
//ps -ef -L|grep pthread
//UID         PID   PPID    LWP  C NLWP STIME TTY          TIME CMD
//root      31228   4192  31228  0    2 17:14 pts/0    00:00:00 [pthread_detach] <defunct>
//root      31228   4192  31229  0    2 17:14 pts/0    00:00:00 [pthread_detach]

//测试2:
//无论是否开启child线程开启pthread_detach
//main线程立刻结束不用for 100, child线程不结束使用for 10
//结果:
//可以看到child线程10秒后自动退出不需要main线程收尸

static void* func(void*ptr)
{
	int i;
	int err;
	printf("child tid:%d\n", gettid());
	err = pthread_detach(pthread_self());
    if (err) {
        fprintf(stderr, "%s\n", strerror(err));
        exit(1);
    }   
	for(i=0; i<10; i++) {
		printf("child ok\n");
		sleep(1);
	}
    pthread_exit((void *)123);
}

int main(int argc, const char *argv[])
{
    int i, err;
    pthread_t tid;
	printf("main tid:%d\n", gettid());
    err = pthread_create(&tid, NULL, func, NULL );
    if (err) {
        fprintf(stderr, "%s\n", strerror(err));
        exit(1);
    }   

	for(i=0; i<100;i++) {
		printf("main ok\n");
		sleep(1);
	}

	pthread_exit(NULL);//必须
	printf("never do\n");
    return 0;
}

