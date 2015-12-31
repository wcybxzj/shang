#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define 	THRNUM		4
#define 	MIN			30000000
#define 	MAX			30000200	

static int num = MIN;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void *thr_primer(void *p)
{
	int i, j, flag;
	while(1) {
		pthread_mutex_lock(&mut);
		if(num > 0) {
			i = num;
			num = 0;
			pthread_mutex_unlock(&mut);
			flag = 1;
			for(j = 2; j < i/2; j++) {
				if(i % j == 0) {
					flag = 0;
					break;
				}
			}
			if(flag)
				printf("%d is a primer\n", i);
		}else if(num == -1) {
			pthread_mutex_unlock(&mut);
			pthread_exit(NULL);
		}else
			pthread_mutex_unlock(&mut);
	}
}


int main()
{
	pthread_t tid[THRNUM];
	int i, err, cnt = MIN;

	for(i = 0; i < THRNUM; i++) {
		err = pthread_create(tid+i, NULL, thr_primer, NULL);
		if(err) {
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}
	
	while(1){
		pthread_mutex_lock(&mut);
		if(num == 0) {
			cnt++;
			num += cnt;
		}
		pthread_mutex_unlock(&mut);
		if(cnt > MAX) {
			pthread_mutex_lock(&mut);
			num = -1;
			pthread_mutex_unlock(&mut);
			break;
		}
	}
	for(i = 0; i < THRNUM; i++)
		pthread_join(tid[i], NULL);

	exit(0);
}
