#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 1024

//线程间也可用pipe通讯但是
//线程间的文件描述符数组是共享的所以2个线程都不能close(fd)
static void * func(void *pd){
	int* mypd = pd;
	int len;
	char buf[SIZE];
	//close(mypd[1]);
	len = read(mypd[0],buf, SIZE);
	printf("%s\n", buf);
	pthread_exit(NULL);
}

int main(){
	int err;
	pthread_t tid;
	int pd[2];
	if(pipe(pd) < 0) {
		perror("pipe():");
		exit(-1);
	}
	err = pthread_create(&tid, NULL, func, (void*) pd);
	if (err) {
		perror("pthread_create():");
		exit(-1);
	}
	//close(pd[0]);
	write(pd[1],"abc",4);
	pthread_join(tid, NULL);
	exit(0);
}
