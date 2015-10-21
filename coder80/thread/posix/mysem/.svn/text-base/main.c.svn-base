#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LEFT		30000000
#define RIGHT		30000200
#define THRNUM		(RIGHT-LEFT+1)
#define N			4

static void *thr_primer(void *p);

int main()
{
	int err,i;	
	pthread_t tid[THRNUM];
	mysem_t *sem;

	sem = mysem_init(N);
	if()
	{

	}

	for(i = LEFT ; i <= RIGHT; i++)
	{
		mysem_sub(sem,1);

		err = pthread_create(tid+(i-LEFT),NULL,thr_primer,(void *)i);		
		if(err)
		{
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
			exit(1);
		}
		
	}

	for(i = LEFT ; i <= RIGHT; i++)
		pthread_join(tid[i-LEFT],NULL);

	mysem_destroy(  );

	exit(0);
}
	

static void *thr_primer(void *p)
{
	int i,j,mark;

	i = (int)p;

	mark = 1;
	for(j = 2; j < i/2; j++)
	{
		if(i % j == 0)
		{
			mark = 0;
			break;
		}
	}
	if(mark)
		printf("%d is a primer.\n",i);

	mysem_add(sem,1);

	pthread_exit(NULL);
}


