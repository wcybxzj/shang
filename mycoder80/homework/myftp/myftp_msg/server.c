#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include "proto1.h"

struct rel_fsm_st
{
	int state;
	int msgid;
	union msg_s2c_un;
	int len;	
	int pos;
	//int64_t count;
	//char *errstr;
};

struct rel_job_st
{	
	int job_state;
	int msgid;
//	pthread_mutex_t mut_job_state;
//	pthread_cond_t cond_job_state;
	struct rel_fsm_st fsm;
//	struct timerval start,end;
};


static void fsm_driver(struct rel_fsm_st *fsm)
{
	int ret;
	
	switch(fsm->state)
	{
		case STATE_R:
			fsm->len = read(fsm->sfd,fsm->buf,BUFSIZE);
			if(fsm->len == 0)
				fsm->state = STATE_T;
			else if(fsm->len < 0)
				{
					if(errno == EAGAIN)
						fsm->state = STATE_R;
					else
					{
						fsm->errstr = "read()";
						fsm->state = STATE_Ex;
					}
				}
			else // len > 0
			{
					fsm->pos = 0;
					fsm->state = STATE_W;
				}
			break;

		case STATE_W:
			ret = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
			if(ret < 0)
			{
				if(errno == EAGAIN)
					fsm->state = STATE_W;
				else
				{
					fsm->errstr = "write()";
					fsm->state = STATE_Ex;
				}
			}
			else
			{
				fsm->count += ret;
				fsm->pos += ret;
				fsm->len -= ret;
				if(fsm->len == 0)
					fsm->state = STATE_R;
				else 
					fsm->state = STATE_W;
			}

			break;

		case STATE_Ex:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;

		case STATE_T:
			/*exit(0),do sth*/
			break;

		default:
			abort();
	}
}

void *thr_relayer(void *p)
{
	int i;
	while(1)
	{
		pthread_mutex_lock(&mut_rel_job);
		for(i = 0 ; i < REL_JOBMAX ; i++)
		{
			if(rel_job[i] != NULL)
			{
				if(rel_job[i]->job_state == STATE_RUNNING)
				{
					fsm_driver(&rel_job[i]->fsm12);			
					fsm_driver(&rel_job[i]->fsm21);
					if(rel_job[i]->fsm12.state == STATE_T 
						&& rel_job[i]->fsm21.state == STATE_T)
						rel_job[i]->job_state = STATE_OVER;	
				}
			}
		}
		pthread_mutex_unlock(&mut_rel_job);
	}
}

static void module_load(void)
{
	pthread_t tid;
	int err;
	err = pthread_create(&tid,NULL,thr_relayer,NULL);
	if(err)
	{
		fprintf(stderr,"pthread_create():%s\n",strerror(err));
		exit(1);
	}
}

int rel_addjob(int msgid)
{
	struct rel_job_st *me;
	int pos;

	//pthread_once(&init_once,module_load);

	if(msgid< 0)
		return -EINVAL;

	me = malloc(sizeof(*me));
	if(me == NULL)
		return -ENOMEM;
	
	me->msgid = msgid;
	me->job_state = STATE_RUNNING;




	
	me->fsm12.sfd = me->fd1;
	me->fsm12.count = 0;
	me->fsm12.state = STATE_R;
	me->fsm21.dfd = me->fd1;
	me->fsm21.count = 0;
	me->fsm21.state = STATE_R;

	pthread_mutex_lock(&mut_rel_job);
	pos = get_free_pos_unlocked();
	if(pos < 0)
	{
		pthread_mutex_unlock(&mut_rel_job);
		fcntl(me->fd1,F_SETFL,me->fd1_save);
		fcntl(me->fd2,F_SETFL,me->fd2_save);
		free(me);
		return -ENOSPC;
	}

	rel_job[pos] = me;
	pthread_mutex_unlock(&mut_rel_job);
	
	return pos;
}

int init_res()
{
	key_t key;
	int msgid;
	key = ftok(KEYPATH,KEYPROJ);
    if(key < 0)
    {
        perror("ftok()");
		return -EBUSY;
    }

    msgid = msgget(key,IPC_CREAT|0600);
	if(msgid < 0)
    {
        perror("msgget()");
		return -EBUSY;
    }
	return msgid;
}

int main(int argc, const char *argv[])
{
	int msgid;
	msgid = init_res();
	rel_addjob(msgid);

	return 0;
}
