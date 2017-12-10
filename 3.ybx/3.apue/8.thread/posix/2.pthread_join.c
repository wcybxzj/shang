#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void *
threadFunc(void *arg)
{
	char *s = (char *) arg;
	//printf("%s", s);
	return (void *) strlen(s);
	//pthread_exit((void *) strlen(s));
}

static void *
threadFunc2(void *arg)
{
	char *s = "abc";
	//return (void *)s;
	pthread_exit((void *)s);
}

int
main(int argc, char *argv[])
{
	pthread_t t1,t2;
	void *res;
	int s;
	s = pthread_create(&t1, NULL, threadFunc, "Hello world\n");
	s = pthread_join(t1, &res);
	printf("Thread returned %ld\n", (long) res);


	s = pthread_create(&t2, NULL, threadFunc2, NULL);
	s = pthread_join(t2, &res);
	printf("Thread returned %s\n", (char*) res);


	exit(EXIT_SUCCESS);
}
