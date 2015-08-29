#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LEFT		30000000
#define RIGHT		30000200
#define THRNUM		4	

static int num = 0;
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;

void *thr_primer(void *);

int main()
{
	int i,err;	
	pthread_t tid[THRNUM];

	for(i = 0; i < THRNUM ;i++)
	{
		err = pthread_create(tid+i,NULL,thr_primer,(void *)i);
		if(err)
		{
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
			exit(1);
		}
	}


	for(i = LEFT; i <= RIGHT; i++)
	{
		pthread_mutex_lock(&mut_num);
		while(num != 0)		
		{
			pthread_mutex_unlock(&mut_num);
			sched_yield();
			pthread_mutex_lock(&mut_num);
		}
		num = i;
		pthread_mutex_unlock(&mut_num);
	}

	pthread_mutex_lock(&mut_num);
	while(num != 0)
	{
		pthread_mutex_unlock(&mut_num);
		sched_yield();
		pthread_mutex_lock(&mut_num);
	}

	num = -1;

	pthread_mutex_unlock(&mut_num);

	for(i = 0; i < THRNUM ;i++)
		pthread_join(tid[i],NULL);
	
	pthread_mutex_destroy(&mut_num);

	exit(0);
}

void *thr_primer(void *p)
{
	int i,j,mark;

	while(1)
	{
		pthread_mutex_lock(&mut_num);

		while(num == 0)
		{
			pthread_mutex_unlock(&mut_num);
			sched_yield();
			pthread_mutex_lock(&mut_num);
		}
		if(num == -1)
		{
			pthread_mutex_unlock(&mut_num);
			break;
		}
		i = num;
		num = 0;

		pthread_mutex_unlock(&mut_num);

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
			printf("[%d]%d is a primer.\n",(int)p,i);
	}

	pthread_exit(NULL);
}


