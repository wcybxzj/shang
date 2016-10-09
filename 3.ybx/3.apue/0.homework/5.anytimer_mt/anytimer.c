#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "anytimer.h"

enum{
	STATE_RUNNING=1,
	STATE_CANCEL,
	STATE_OVER,
	STATE_PAUSE
};

struct at_job_st{
	int job_state;
	int sec;//这个时间不变,做周期性任务 adt_addjob_repeat
	int time_remain;//用这个递减
	int repeat;//0非周期 1是周期性
	at_jobfunc_t *jobp;
	void *arg;
	pthread_mutex_t mut;
	pthread_cond_t cond;
};

static pthread_t tid;
static struct at_job_st *job[JOB_MAX];
static pthread_once_t once_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;

static void* init_action(void *p)
{
	int i;
	pthread_t tid1;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	while (1) {
		pthread_mutex_lock(&mut_job);
		for(i = 0 ; i < JOB_MAX; i++)
		{
			if(job[i] != NULL)
			{
				pthread_mutex_lock(&job[i]->mut);
				if (job[i]->job_state==STATE_RUNNING) {
					job[i]->time_remain--;
					if (job[i]->time_remain == 0) {
						pthread_create(&tid1, &attr, job[i]->jobp, job[i]->arg);
						//job[i]->jobp(job[i]->arg);//todo
						if (job[i]->repeat == REPEAT) {
							job[i]->time_remain = job[i]->sec;
						}else{
							job[i]->job_state = STATE_OVER;
							pthread_cond_broadcast(&job[i]->cond);
						}
					}
				}
				pthread_mutex_unlock(&job[i]->mut);
			}
		}
		pthread_mutex_unlock(&mut_job);
		sleep(1);
		write(1, "$", 1);

	}
}

void module_unload()
{
	int i;
	pthread_cancel(tid);
	pthread_join(tid, NULL);

	pthread_mutex_lock(&mut_job);
	for(i = 0 ; i < JOB_MAX ; i++){
		pthread_mutex_destroy(&job[i]->mut);
		pthread_cond_destroy(&job[i]->cond);
		free(job[i]);
	}
	pthread_mutex_unlock(&mut_job);
	pthread_mutex_destroy(&mut_job);
}

void module_load()
{
	int err;
	err = pthread_create(&tid, NULL, init_action, NULL);
	if (err < 0) {
		fprintf(stderr, "pthread_create err %s\n", strerror(err));
		exit(1);
	}
	atexit(module_unload);
}

static int  get_free_pos_unlock(void){
	int i;
	for (i = 0; i < JOB_MAX; i++) {
		if (job[i] == NULL) {
			return i;
		}
	}
	return -1;
}

int at_addjob(int sec,at_jobfunc_t *jobp,void *arg, int is_repeat){
	int pos;
	struct  at_job_st *me;
	pthread_once(&once_init, module_load);

	me = malloc(sizeof(*me));
	if (NULL==me) {
		return -ENOMEM;
	}
	me->job_state =  STATE_RUNNING;
	me->sec = sec;
	me->time_remain = sec;
	me->jobp = jobp;
	me->arg = arg;
	me->repeat = is_repeat;
	pthread_mutex_init(&me->mut, NULL);
	pthread_cond_init(&me->cond, NULL);

	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlock();
	if (pos < 0) {
		pthread_mutex_unlock(&mut_job);
		pthread_mutex_destroy(&me->mut);
		pthread_cond_destroy(&me->cond);
		free(me);
		return -ENOSPC;
	}
	job[pos] = me;
	pthread_mutex_unlock(&mut_job);
	return pos;
}

int at_addjob_repeat(int sec,at_jobfunc_t *jobp,void *arg){

}

int at_canceljob(int id){
	pthread_mutex_lock(&mut_job);
	if (id < 0 || id >= JOB_MAX || job[id] == NULL) {
		pthread_mutex_unlock(&mut_job);
		return -EINVAL;
	}

	pthread_mutex_lock(&job[id]->mut);
	if (job[id]->job_state == STATE_OVER) {
		pthread_mutex_unlock(&mut_job);
		pthread_mutex_unlock(&job[id]->mut);
		return -EBUSY;
	}

	job[id]->job_state = STATE_CANCEL;
	//强制把repeat 变成0，要不周期性的job wait无法收尸
	job[id]->repeat = NOREPEAT;
	pthread_mutex_unlock(&job[id]->mut);
	pthread_mutex_unlock(&mut_job);
	return 0;
}

//无论多少个资源,只要设置进来就会最后回收
int at_waitjob(int id){
	pthread_mutex_lock(&mut_job);
	if (id < 0 || id >= JOB_MAX || job[id] == NULL) {
		pthread_mutex_unlock(&mut_job);
		return -EINVAL;
	}
	pthread_mutex_unlock(&mut_job);

	pthread_mutex_lock(&job[id]->mut);
	if (job[id]->repeat==REPEAT) {
		pthread_mutex_unlock(&job[id]->mut);
		return -EBUSY;
	}
	printf("waitjob id %d 判断状态改变\n", id);
	while(job[id]->job_state==STATE_RUNNING) {
		printf("waitjob id %d 等待状态改变\n", id);
		pthread_cond_wait(&job[id]->cond, &job[id]->mut);
	}
	pthread_mutex_unlock(&job[id]->mut);

	pthread_mutex_lock(&mut_job);
	if (job[id]->job_state == STATE_CANCEL || job[id]->job_state == STATE_OVER) {
		printf("waitjob id %d  释放资源\n", id);
		pthread_mutex_destroy(&job[id]->mut);
		pthread_cond_destroy(&job[id]->cond);
		free(job[id]);
		job[id]=NULL;
	}
	pthread_mutex_unlock(&mut_job);
	return 0;
}

int at_stopjob(int id){
	pthread_mutex_lock(&job[id]->mut);
	if (job[id]->job_state != STATE_RUNNING) {
		pthread_mutex_unlock(&job[id]->mut);
		return -1;
	}
	job[id]->job_state = STATE_PAUSE;
	pthread_mutex_unlock(&job[id]->mut);
}

int at_resumejob(int id){
	pthread_mutex_lock(&job[id]->mut);
	if (job[id]->job_state != STATE_PAUSE) {
		pthread_mutex_unlock(&job[id]->mut);
		return -1;
	}
	job[id]->job_state = STATE_RUNNING;
	pthread_mutex_unlock(&job[id]->mut);
}

