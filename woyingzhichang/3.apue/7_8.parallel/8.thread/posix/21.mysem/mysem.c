#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mysem.h"

typedef struct _mysem_st {
	int num;//线程并发数
	pthread_cond_t cond;
	pthread_mutex_t mut;
} mysem_st;

mysem_t* mysem_init(int init_num){
	mysem_st* me = malloc(sizeof(mysem_st));
	if (me==NULL) {
		return NULL;
	}
	me->num = init_num;
	pthread_cond_init(&me->cond, NULL);
	pthread_mutex_init(&me->mut, NULL);
	return me;
}

int mysem_add(mysem_t*ptr, int num){
	mysem_st *me = ptr;
	pthread_mutex_lock(&me->mut);
	me->num += num;
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	return num;
}

int mysem_sub(mysem_t*ptr, int num){
	mysem_st* me = ptr;
	pthread_mutex_lock(&me->mut);
	while (num > me->num) {
		pthread_cond_wait(&me->cond, &me->mut);
	}
	me->num -= num;
	pthread_mutex_unlock(&me->mut);
	return num;
}

void mysem_destory(mysem_t*ptr){
	mysem_st* me = ptr;
	pthread_cond_destroy(&me->cond);
	pthread_mutex_destroy(&me->mut);
	free(me);
}
