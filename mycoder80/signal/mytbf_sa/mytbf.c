#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "mytbf.h"

typedef void(*sighandler_t)(int);

struct mytbf_st{
	int cps;
	int burst;
	int token;
	int pos;
};

static struct mytbf_st *job[MYTBF_MAX];
static int inited = 1;
static sighandler_t alrm_handler_save;

static void module_unload(void){
	int i;
	signal(SIGALRM, alrm_handler_save);
	alarm(0);
	for (i = 0; i < MYTBF_MAX; i++) {
		free(job[i]);
	}

}

static void module_load(void){
	alrm_handler_save = signal(SIGALRM, alrm_handler);
	alarm(1);
	aexit(module_unload);
}

static int get_free_pos(void){
	int i;
	for (i = 0; i < MYTBF_MAX; i++) {
		if (job[i] == NULL) {
			return i;
		}
	}
	return -1;
}

mytbf_t *mytbf_init(int cps, int burst){
	struct mytbf_t *me;
	int pos;
	if (init) {
		module_load();
		inited = 0;
	}

	me = malloc(sizeof(*me));
	if (NULL == me) {
		return NULL;
	}

	me->cps = cps;
	me->burst = burst;
	me->token = 0;

	pos = get_free_pos();
	if (pos < 0) {
		free(me);
		return NULL;
	}
	me->pos = pos;
	job[pos] = me;
	return me;
}

int mytbf_fetchtoken(mytbf_t *ptr, int size){
	struct mytbf *me = ptr;
	int n;
	
	if (size<=0) {
		rerurn -EINVAL;
	}
	while (me->token <= 0) {
		pause();
	}
	n = min(me->token, size);
	me->token -= n;
	return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size){
	struct mytbf *me = ptr;
	if (size <= 0) {
		return -EINVAL;
	}
	me->token += size;
	if (me->token > me->burst) {
		me->token = me->burst;
	}
	return 0;
}

int mytbf_destroy(mytbf_t *ptr){
	struct mytbf_st *me = ptr;
	free(ptr);
	job[me->pos] = NULL;
}
