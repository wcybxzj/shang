#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include "mytbf.h"

typedef struct _mytbf_st {
	int cps;
	int burst;
	volatile sig_atomic_t token;
	int pos;
	pthread_mutex_t mut;
} mytbf_st;


static int inited = 0;
static mytbf_st *job[MYTBF_MAX];

static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mut_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t once_init = PTHREAD_ONCE_INIT;
static pthread_t tid;

static int get_free_pos_unlocked(void)
{
	int i;
	for (i = 0; i < MYTBF_MAX; i++) {
		if (job[i]==NULL) {
			return i;
		}
	}
	return -1;
}

static void* thr_alrm(void *p)
{
	int i;
	while (1) {
		for (i = 0; i < MYTBF_MAX; i++) {
			pthread_mutex_lock(&mut_job);
			if (job[i]!=NULL) {
				job[i]->token += job[i]->cps;
				if (job[i]->token > job[i]->burst) {
					job[i]->token = job[i]->burst;
				}
			}
			pthread_mutex_unlock(&mut_job);
		}
		sleep(1);
	}
}

static void module_unload(void)
{
	int i;
	pthread_cancel(tid);
	pthread_join(tid, NULL);
	pthread_mutex_lock(&mut_job);
	for (i = 0; i < MYTBF_MAX; i++) {
		if (job[i]!=NULL) {
			pthread_mutex_destroy(&job[i]->mut);
			free(job[i]);
			job[i] = NULL;
		}
	}
	pthread_mutex_unlock(&mut_job);
	pthread_mutex_destroy(&mut_job);

}

static void module_load(void)
{
	int err;
	err = pthread_create(&tid, NULL, thr_alrm, NULL);
	if (err) {
		fprintf(stderr, "pthread_create(): %s\n", strerror(err));
		exit(1);
	}
	atexit(module_unload);
}

mytbf_t *mytbf_init(int cps, int burst){
	mytbf_st * me;
	int pos;

	//pthread_mutex_lock(&mut_init);
	//if (!inited) {
	//	module_load();
	//	inited=1;
	//}
	//pthread_mutex_unlock(&mut_init);
	pthread_once(&once_init, module_load);

	me = malloc(sizeof(mytbf_st));
	if (NULL == me) {
		return NULL;
	}
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	pthread_mutex_init(&me->mut, NULL);

	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlocked();
	if (pos < 0) {
		pthread_mutex_unlock(&mut_job);
		pthread_mutex_destroy(&me->mut);
		free(me);
		return NULL;
	}
	me->pos = pos;
	job[pos] = me;
	pthread_mutex_unlock(&mut_job);
	return me;
}

static int min(int a, int b)
{
	return (a<b)?a:b;
}

int mytbf_fetchtoken(mytbf_t *ptr, int size){
	int n;
	mytbf_st *me = ptr;
	if (size<=0) {
		return -EINVAL;//参数非法
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
	mytbf_st *me = ptr;
	if (size <= 0) {
		return -EINVAL;
	}
	pthread_mutex_lock(&me->mut);
	me->token+=size;
	if (me->token > me->burst) {
		me->token =  me->burst;
	}
	pthread_mutex_unlock(&me->mut);

	return size;
}

int mytbf_destroy(mytbf_t *ptr){
	mytbf_st *me = ptr;
	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	pthread_mutex_unlock(&mut_job);

	pthread_mutex_destroy(&mut_job);
	free(ptr);
	return 0;
}
