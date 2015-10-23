#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "mytbf.h"

struct mytbf_st{
	int cps;
	int burst;
	int token;
	int pos;
	pthread_mutex_t mut;
};

static struct mytbf_st *job[MYTBF_MAX];
static pthread_mutex_t  mut_job = PTHREAD_MUTEX_INITIALIZER;
static int inited = 1;
static pthread_t tid_alrm;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static int min(int a, int b){
	if (a < b) {
		return a;
	}
	return b;
}

static void* thr_alrm(void *p){
	int i;

	while (1) {
		pthread_mutex_lock(&mut_job);//job
		for (i = 0; i < MYTBF_MAX; i++) {
			if (job[i] != NULL) {
				pthread_mutex_lock(&job[i]->mut);//token
				job[i]->token += job[i]->cps;
				if (job[i]->token > job[i]->burst) {
					job[i]->token = job[i]->burst;
				}
				pthread_mutex_unlock(&job[i]->mut);
			}
		}
		pthread_mutex_unlock(&mut_job);
		sleep(1);
	}

}

static void module_unload(void){
	int i;
	//回收加token的线程
	pthread_cancel(tid_alrm);
	pthread_join(tid_alrm, NULL);

	for(i = 0 ; i < MYTBF_MAX ; i++){
		if (job[i]!=NULL) {
			mytbf_destroy(job[i]);
		}
	}
	pthread_mutex_destroy(&mut_job);
}

static void module_load(void){
	int err;
	err = pthread_create(&tid_alrm, NULL, thr_alrm, NULL);
	if (err) {
		fprintf(stderr, "pthread_create():%s\n", strerror(errno));
		exit(1);
	}
	atexit(module_unload);
}

//函数本身本该锁没加,提示要加锁再用
static int get_free_pos_unlock(void){
	int i;
	for (i = 0; i < MYTBF_MAX; i++) {
		if (job[i] == NULL) {//本该加锁
			return i;
		}
	}
	return -1;
}

mytbf_t *mytbf_init(int cps, int burst){
	struct mytbf_st *me;
	int pos;
	pthread_once(&init_once, module_load);

	me = malloc(sizeof(*me));
	if (NULL == me) {
		return NULL;
	}

	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	pthread_mutex_init(&me->mut, NULL);

	//lock的内容都和job有关系
	//一定lock要短这部分是现成并发的瓶颈
	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlock();
	if (pos < 0) {
		pthread_mutex_unlock(&mut_job);
		free(me);
		return NULL;//临界区的跳转必须先 unlock避免死锁
	}
	me->pos = pos;
	job[pos] = me;
	pthread_mutex_unlock(&mut_job);

	return me;
}

//token 和 thr_alrm 中增加token冲突
int mytbf_fetchtoken(mytbf_t *ptr, int size){
	struct mytbf_st *me = ptr;
	int n;

	if (size<=0) {
		return -EINVAL;
	}


	pthread_mutex_lock(&me->mut);
	while (me->token <= 0) {
		pthread_mutex_unlock(&me->mut);
		sched_yield();
		pthread_mutex_lock(&me->mut);
	}

	n = min(me->token, size);
	me->token -= n;
	pthread_mutex_unlock(&me->mut);
	return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size){
	struct mytbf_st *me = ptr;
	if (size <= 0) {
		return -EINVAL;
	}
	pthread_mutex_lock(&me->mut);
	me->token += size;
	if (me->token > me->burst) {
		me->token = me->burst;
	}
	pthread_mutex_unlock(&me->mut);
	return 0;
}

int mytbf_destroy(mytbf_t *ptr){
	struct mytbf_st *me = ptr;
	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	pthread_mutex_unlock(&mut_job);

	pthread_mutex_destroy(&me->mut);
	free(ptr);
	return 0;
}
