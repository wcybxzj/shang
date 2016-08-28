#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mypipe.h"
#include "queue.h"
#include <sys/syscall.h>   

struct mypipe_st{
	Q *queue;
	int count_rd;
	int count_wr;
	pthread_cond_t cond;
	pthread_mutex_t mut;
};

mypipe_t *mypipe_init(){
	struct mypipe_st *me = NULL;
	Q *you = NULL;
	you = q_create();
	if (you== NULL) {
		return Q_CREATE_FAIL;
	}
	me = malloc(sizeof(*me));
	if (me == NULL) {
		return MYPIPE_INIT_FAIL;
	}
	me->queue = you;
	me->count_rd = 0;
	me->count_wr = 0;
	pthread_mutex_init(&me->mut, NULL);
	pthread_cond_init(&me->cond, NULL);
	return me;
}

int mypipe_register(mypipe_t *ptr, int opmap){
	struct mypipe_st *me = ptr;
	if (!(opmap|MYPIPE_READ) && !(opmap|MYPIPE_WRITE)){
		return -1;
	}
	pthread_mutex_lock(&me->mut);
	if(opmap & MYPIPE_READ){
		me->count_rd++;
	}
	if(opmap & MYPIPE_WRITE){
		me->count_wr++;
	}
	pthread_cond_broadcast(&me->cond);
	while(me->count_rd<=0 || me->count_wr<=0){
		pthread_cond_wait(&me->cond, &me->mut);
	}
	pthread_mutex_unlock(&me->mut);
	return 0;
}

int mypipe_unregister(mypipe_t *ptr, int opmap){
	struct mypipe_st *me = ptr;
	if (!(opmap|MYPIPE_READ) && !(opmap|MYPIPE_WRITE)){
		return -1;
	}
	pthread_mutex_lock(&me->mut);
	if(opmap & MYPIPE_READ){
		me->count_rd--;
	}
	if(opmap & MYPIPE_WRITE){
		me->count_wr--;
		//printf("=%d=\n", me->count_wr);
	}
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	return 0;
}

ssize_t mypipe_read(mypipe_t *ptr, void *buf, size_t count){
	//sleep(1000);
	int i, ret;
	struct mypipe_st *me = ptr;
	datatype *buff = buf;
	pthread_mutex_lock(&me->mut);
	while(q_isempty(me->queue) && me->count_wr>0){
		//printf("[%d]q_isempty:%d\n", syscall(SYS_gettid) , q_isempty(me->queue));
		//printf("wr:%d\n", me->count_wr);
		//printf("[%d]child wait\n", syscall(SYS_gettid));
		pthread_cond_wait(&me->cond, &me->mut);
		//printf("[%d]child wait over\n", syscall(SYS_gettid));
		//printf("[%d]q_isempty:%d\n", syscall(SYS_gettid) , q_isempty(me->queue));
	}
	if(q_isempty(me->queue) && me->count_wr<=0){
		//printf("[%d]q_isempty:%d\n", syscall(SYS_gettid) , q_isempty(me->queue));
		pthread_mutex_unlock(&me->mut);
		//printf("[%d]child empty and 0\n", syscall(SYS_gettid));
		return 0;
	}
	for(i=0; i<count; i++){
		if( (ret = q_dequeue(me->queue, buff+i)) < 0){
			//printf("[%d]q_isempty:%d\n", syscall(SYS_gettid) , q_isempty(me->queue));
			//printf("[%d]q_dequeue is failed\n", syscall(SYS_gettid));
			break;
		}
	}
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	return i;
}

ssize_t mypipe_write(mypipe_t *ptr, const void *buf, size_t count){
	int i, ret;
	struct mypipe_st *me = ptr;
	datatype *buff = buf;
	pthread_mutex_lock(&me->mut);
	while(q_isfull(me->queue)&& me->count_rd>0){
		pthread_cond_wait(&me->cond, &me->mut);
	}
	if(q_isfull(me->queue) && me->count_rd<=0){
		pthread_mutex_unlock(&me->mut);
		return 0;
	}
	//printf("count:%d\n", count);
	for(i=0; i<count; i++){
		if(q_enqueue(me->queue, buff+i) < 0){
			break;
		}
	}
	//q_travel(me->queue);
	//printf("==============\n");
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	return i;
}

void mypipe_destroy(mypipe_t *ptr){
	struct mypipe_st *me = ptr;
	q_destroy(me->queue);
	me->queue = NULL;
	pthread_mutex_destroy(&me->mut);
	pthread_cond_destroy(&me->cond);
	free(me);
	me = NULL;
}
