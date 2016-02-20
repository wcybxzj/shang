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
#include <errno.h>
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
};

static int inited = 0;
static struct at_job_st *job[JOB_MAX];
struct sigaction sa, osa;

static void alrm_action(int s, siginfo_t *infop,void *unused )
{
	int i;

	if(infop->si_code != SI_KERNEL)
		return ;

	for(i = 0 ; i < JOB_MAX; i++)
	{
		if(job[i] != NULL)
		{
			if (job[i]->job_state==STATE_RUNNING) {
				job[i]->time_remain--;
				if (job[i]->time_remain == 0) {
					job[i]->jobp(job[i]->arg);
					if (job[i]->repeat == REPEAT) {
						job[i]->time_remain = job[i]->sec;
					}else{
						job[i]->job_state = STATE_OVER;
					}
				}
			}
		}
	}
}

void module_unload()
{
	int i;
	struct itimerval itv;
	itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;
	if(setitimer(ITIMER_REAL,&itv,NULL) < 0)
	{
		perror("setitimer");
		exit(1);
	}

	if(sigaction(SIGALRM,&osa,NULL) < 0)
	{
		perror("sigaction()");
		exit(1);
	}

	for(i = 0 ; i < JOB_MAX ; i++)
		free(job[i]);
}

void module_load()
{
	sa.sa_sigaction = alrm_action;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	if(sigaction(SIGALRM,&sa, &osa) < 0)
	{
		perror("sigaction()");
		exit(1);
	}
	/*if error*/

	struct itimerval itv;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;	
	if(setitimer(ITIMER_REAL,&itv,NULL) < 0)
	{
		perror("setitimer()");
		exit(1);
	}


	atexit(module_unload);
}

static int  get_free_pos(void){
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
	if (!inited) {
		module_load();
		inited = 1;
	}

	pos = get_free_pos();
	if (pos < 0) {
		return -ENOSPC;
	}

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
	job[pos] = me;
}

int at_addjob_repeat(int sec,at_jobfunc_t *jobp,void *arg){

}

int at_canceljob(int id){
	if (id < 0 || id >= JOB_MAX || job[id] ==NULL) {
		return -EINVAL;
	}
	if (job[id]->job_state == STATE_RUNNING) {
		return -ECANCELED;
	}
	if (job[id]->job_state == STATE_OVER) {
		return -EBUSY;
	}
	if (job[id]->repeat == REPEAT) {
	  job[id]->repeat = NO_REPEAT;
	}
	job[id]->job_state = STATE_CANCEL;
	return 0;
}

int at_waitjob(int id){
	if (id < 0 || id >= JOB_MAX || job[id] == NULL) {
		return -EINVAL;
	}

	if (job[id]->repeat==REPEAT) {
		return -EBUSY;
	}

	while(job[id]->job_state==STATE_RUNNING) {
		write(1, ".", 1);
		//printf("waitjob pause id %d\n", id);
		pause();
	}

	if (job[id]->job_state == STATE_CANCEL || job[id]->job_state == STATE_OVER) {
		//printf("waitjob free id %d \n", id);
		free(job[id]);
		job[id]=NULL;
	}
	return 0;
}


int at_stopjob(int id){
	if (job[id]->job_state != STATE_RUNNING) {
		return -1;
	}
	job[id]->job_state = STATE_PAUSE;
	return 0;
}

int at_resumejob(int id){
	if (job[id]->job_state != STATE_PAUSE) {
		return -1;
	}
	job[id]->job_state = STATE_RUNNING;
	return 0;
}

