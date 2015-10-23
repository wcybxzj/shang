#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
static int *int_p =NULL;

static void* thr_alrm(void *p){
	int_p = malloc(sizeof(int));
	*int_p = 123;
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	pthread_t tid_alrm;

	pthread_create(&tid_alrm, NULL, thr_alrm, NULL);

	pthread_join(tid_alrm, NULL);


	printf("%d\n", *int_p);

	return 0;
}
