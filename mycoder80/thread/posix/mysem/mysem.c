#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mysem.h"

struct mysem_st{
	int value;
	pthread_mutex_t mut;
	pthread_cond_t cond;
};

mysem_t *mysem_init(int initval){
	struct mysem_st *me;
	me = malloc(sizeof(*me));
	if (NULL == me) {
		return NULL;
	}
	me->value = initval;
	pthread_mutex_init(&me->mut, NULL);
	pthread_cond_init(&me->cond, NULL);
	return me;
}

int mysem_add(mysem_t *, int){

}

int mysem_sub(mysem_t *, int){

}

int mysem_destroy(mysem_t *ptr){
	struct mysem_st *me = ptr;
	pthread_cond_destroy(me->)
	pthread_mutex_destroy(me->mut);
	free(me);
}
