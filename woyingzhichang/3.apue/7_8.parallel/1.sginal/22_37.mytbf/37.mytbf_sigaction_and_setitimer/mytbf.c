#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "mytbf.h"

typedef struct _mytbf_st {
	int cps;
	int burst;
	//int token;
	volatile sig_atomic_t token;
	int pos;
} mytbf_st;

static mytbf_st *job[MYTBF_MAX];
static inited = 0;
typedef void (*sighandler_t)(int);
static struct sigaction osa;

static int get_free_pos(void)
{
	int i;
	for (i = 0; i < MYTBF_MAX; i++) {
		if (job[i]==NULL) {
			return i;
		}
	}
	return -1;
}

static void alrm_handler(int s, siginfo_t *info, void*ucp)
{
	int i;

	if (info->si_code!=SI_KERNEL) {
		return;
	}

	for (i = 0; i < MYTBF_MAX; i++) {
		if (job[i]!=NULL) {
			job[i]->token += job[i]->cps;
			if (job[i]->token > job[i]->burst) {
				job[i]->token = job[i]->burst;
			}
		}
	}
}

static void module_unload(void)
{
	int i;
	struct itimerval itv;

	if (sigaction(SIGALRM, &osa, NULL)<0) {
		perror("sigaction():");
		exit(1);
	}

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &itv, NULL)<0) {
		perror("setitimer():");
		exit(2);
	}

	for (i = 0; i < MYTBF_MAX; i++) {
		free(job[i]);
		job[i] = NULL;
	}
}

static void module_load(void)
{
	struct sigaction sa;
	struct itimerval itv;

	sa.sa_sigaction = alrm_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	if ( sigaction(SIGALRM, &sa, &osa) < 0) {
		perror("sigaction():");
		exit(1);
	}

	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &itv, NULL)<0) {
		perror("setitimer():");
		exit(2);
	}

	atexit(module_unload);
}

mytbf_t *mytbf_init(int cps, int burst){
	int pos;
	mytbf_st * me;
	//即使多次调用mytbf_init 生成多个令牌桶,load_module也只运行一次
	//因为一个进程只能有一个定时器生效
	if (!inited) {
		module_load();
		inited = 1;
	}
	pos = get_free_pos();
	if (pos < 0) {
		return NULL;
	}
	me = malloc(sizeof(mytbf_st));
	if (NULL == me) {
		return NULL;
	}
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	me->pos = pos;
	job[pos] = me;
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

	//不用if,而用while是因为来信号不一定token>0
	//相当于一种阻塞的实现,保证token大于0才向下执行
	while (me->token <= 0) {
		pause();
	}
	n = min(me->token, size);
	me->token -= n;
	return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size){
	mytbf_st *me = ptr;
	if (size <= 0) {
		return -EINVAL;
	}
	me->token+=size;
	if (me->token > me->burst) {
		me->token =  me->burst;
	}
	return size;
}

int mytbf_destroy(mytbf_t *ptr){
	mytbf_st *me = ptr;
	job[me->pos] = NULL;
	free(ptr);
	return 0;
}
