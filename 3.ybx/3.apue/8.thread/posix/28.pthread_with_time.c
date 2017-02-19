#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#define MAX 5

pid_t gettid()
{
     return syscall(SYS_gettid);
}

int my_rand(int range){
    return rand()%range;
}

static void* func(void*ptr)
{
	struct timeval start, end;
	gettimeofday( &start, NULL );
	int i;
	int err;
	printf("child tid:%d\n", gettid());

	int max = my_rand(10);

	for(i=0; i<max; i++) {
		//printf("child ok\n");
		sleep(1);
	}

	gettimeofday( &end, NULL );
    printf("tid:%d,max:%d, used-time:%lu.%lu sec\n",\
            gettid(),max,end.tv_sec-start.tv_sec, end.tv_usec-start.tv_usec);
    pthread_exit((void *)123);
	//exit(0);
	//return;
}

//目的:看main线程的计时和其他线程之间是否互相干扰
int main(int argc, const char *argv[])
{
    int i, err;
    pthread_t tids[MAX];
	
    srand(time(NULL));

	struct timeval start, end;
	gettimeofday( &start, NULL );
	for (i = 0; i < MAX; i++) {
		err = pthread_create(&tids[i], NULL, func, NULL );
		if (err) {
			fprintf(stderr, "%s\n", strerror(err));
			exit(1);
		}   
	}

	for (i = 0; i < MAX; i++) {
		pthread_join(tids[i], NULL);
		gettimeofday( &end, NULL );
		printf("main join i:%d, used-time:%lu.%lu sec\n",\
				i, end.tv_sec-start.tv_sec, end.tv_usec-start.tv_usec);
	}

	gettimeofday( &end, NULL );
    printf("main tid:%d, used-time:%lu.%lu sec\n",\
            gettid(), end.tv_sec-start.tv_sec, end.tv_usec-start.tv_usec);
	pthread_exit(NULL);//必须
    //return 0;
	//exit(0);
}

