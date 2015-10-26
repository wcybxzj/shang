#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "anytimer.h"

enum{
	STATE_RUNNING=1,
	STATE_CANCEL,
	STATE_OVER
}

struct at_job_st{
	int job_state;
	int sec;
	int time_remain;
	at_jobfunc_t *jobp;
	void *arg;
};

int at_addjob(int sec,at_jobfunc_t *jobp,void *arg){
	
}

int at_canceljob(int id){

}

int at_waitjob(int id){

}
