#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int glob;

static void* func(void *p)
{
	int i = *(int *)p;
	printf("in thread i:%d\n", i);
	printf("in thread glob:%d\n", glob);
	i=333;
	glob=444;
}

//in thread i:123
//in thread glob:123
//main i:123
//main glob:999

int main(int argc, const char *argv[])
{
	int i=111;
	glob=222;
	pthread_t tid;
	pthread_create(&tid, NULL, func, &i);
	pthread_join(tid, NULL);
	sleep(1);
	printf("main i:%d\n",i);
	printf("main glob:%d\n",glob);
	exit(0);
}
