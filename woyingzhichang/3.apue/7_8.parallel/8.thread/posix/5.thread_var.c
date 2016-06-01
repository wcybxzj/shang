#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int err;
int glob;

static void* func(void *p)
{
	int i = *(int *)p;
	printf("in thread i:%d\n", i);
	i=789;
	glob=999;
}

int main(int argc, const char *argv[])
{
	int i=123;
	glob=456;
	pthread_t tid;
	pthread_create(&tid, NULL, func, &i);
	pthread_join(tid, NULL);
	printf("%d\n",i);
	printf("glob:%d\n",glob);
	exit(0);
}
