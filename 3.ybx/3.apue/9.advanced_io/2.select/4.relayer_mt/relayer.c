#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>

#include "relayer.h"

#define BUFSIZE		1024

enum
{
	STATE_R=1,
	STATE_W,
	STATE_AUTO,
	STATE_Ex,
	STATE_T
};
		
struct rel_fsm_st
{
	int state;
	int sfd;
	int dfd;
	char buf[BUFSIZE];
	int len;	
	int pos;
	int64_t count;
	char *errstr;
};
struct rel_job_st
{	
	int job_state;
	int fd1,fd2;
	int fd1_save,fd2_save;
	struct rel_fsm_st fsm12,fsm21;
};

static struct rel_job_st *rel_job[REL_JOBMAX];
static pthread_mutex_t mut_rel_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

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

int max(int fd1, int fd2){
	if(fd1>fd2){
		return fd1;
	}else{
		return fd2;
	}
}

void *thr_relayer(void *p)
{
	int i;
	fd_set rset,wset;
	int maxfd=0;

	while(1)
	{
		//布置监视任务
		printf("布置监视任务\n");
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		for(i = 0 ; i < REL_JOBMAX ; i++)
		{
			if(rel_job[i] != NULL)
			{
				maxfd = (maxfd, max(rel_job[i]->fd1, rel_job[i]->fd2));
				if(rel_job[i]->job_state == STATE_RUNNING)
				{
					if(rel_job[i]->fsm12.state == STATE_R){
						printf("i:%d,fsm12 state_r\n",i);
						FD_SET(rel_job[i]->fsm12.sfd,&rset);
					}   
					if(rel_job[i]->fsm12.state == STATE_W){
						printf("i:%d,fsm12 state_w\n",i);
						FD_SET(rel_job[i]->fsm12.dfd,&wset);
					}   
					if(rel_job[i]->fsm21.state == STATE_R){
						printf("i:%d,fsm21 state_r\n",i);
						FD_SET(rel_job[i]->fsm21.sfd,&rset);
					}   
					if(rel_job[i]->fsm21.state == STATE_W){
						printf("i%d,fsm21 state_w\n",i);
						FD_SET(rel_job[i]->fsm21.dfd,&wset);                
					}   
					//为了处理无条件Ex->T
					if(rel_job[i]->fsm12.state > STATE_AUTO){
						fsm_driver(&rel_job[i]->fsm12);        
					}
					if(rel_job[i]->fsm21.state > STATE_AUTO){
						fsm_driver(&rel_job[i]->fsm21);        
					}
				}
			}
		}

		//监视
		printf("block max fd %d\n", maxfd);
		if(select(maxfd+1,&rset,&wset,NULL,NULL)<0)                      
		{   
			if(errno == EINTR)
				continue;
			perror("select()");
			exit(1);
		}   

		/*查看监视结果*/                                                            
		printf("unblock\n");
		pthread_mutex_lock(&mut_rel_job);
		for(i = 0 ; i < REL_JOBMAX ; i++)
		{
			if(rel_job[i] != NULL)
			{
				if(rel_job[i]->job_state == STATE_RUNNING)
				{
					if(FD_ISSET(rel_job[i]->fd1,&rset) || FD_ISSET(rel_job[i]->fd2,&wset)){
						printf("fsm12\n");
						fsm_driver(&rel_job[i]->fsm12);        
					}
					if(FD_ISSET(rel_job[i]->fd2,&rset) || FD_ISSET(rel_job[i]->fd1,&wset)){
						printf("fsm21\n");
						fsm_driver(&rel_job[i]->fsm21);
					}
				}
			}
		}
		pthread_mutex_unlock(&mut_rel_job);
	}
}

//module_unload()

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

static int get_free_pos_unlocked(void)
{
	int i;
	for(i = 0 ; i < REL_JOBMAX; i++)
	{
		if(rel_job[i] == NULL)
			return i;
	}
	return -1;
}

int rel_addjob(int fd1,int fd2)
{
	struct rel_job_st *me;
	int pos;

	if(fd1 < 0 || fd2 < 0)
		return -EINVAL;

	me = malloc(sizeof(*me));
	if(me == NULL)
		return -ENOMEM;

	me->fd1 = fd1;
	me->fd2 = fd2;
	me->job_state = STATE_RUNNING;
	//使用阻塞IO完全可以，select可以具体获取那个fd的什么类型事件
	//me->fd1_save = fcntl(me->fd1,F_GETFL);
	//fcntl(me->fd1,F_SETFL,me->fd1_save|O_NONBLOCK);
	//me->fd2_save = fcntl(me->fd2,F_GETFL);
	//fcntl(me->fd2,F_SETFL,me->fd2_save|O_NONBLOCK);

	me->fsm12.sfd = me->fd1;
	me->fsm12.dfd = me->fd2;
	me->fsm12.count = 0;
	me->fsm12.state = STATE_R;
	me->fsm21.sfd = me->fd2;
	me->fsm21.dfd = me->fd1;
	me->fsm21.count = 0;
	me->fsm21.state = STATE_R;

	pthread_mutex_lock(&mut_rel_job);
	pos = get_free_pos_unlocked();
	if(pos < 0)
	{
		pthread_mutex_unlock(&mut_rel_job);
		//fcntl(me->fd1,F_SETFL,me->fd1_save);
		//fcntl(me->fd2,F_SETFL,me->fd2_save);
		free(me);
		return -ENOSPC;
	}

	rel_job[pos] = me;
	pthread_mutex_unlock(&mut_rel_job);

	return pos;
}

static void fetch_stat_unlocked(int id,struct rel_stat_st *st)
{
	st->fd1 = rel_job[id]->fd1;
	st->fd2 = rel_job[id]->fd2;
	st->state = rel_job[id]->job_state;
	st->count12 = rel_job[id]->fsm12.count;
	st->count21 = rel_job[id]->fsm21.count;
}

int rel_statjob(int id,struct rel_stat_st *st)
{
	pthread_mutex_lock(&mut_rel_job);
	if(id < 0 || id >= REL_JOBMAX || rel_job[id] == NULL)
	{
		pthread_mutex_unlock(&mut_rel_job);
		return -EINVAL;
	}

	if(st != NULL)
		fetch_stat_unlocked(id,st);	

	pthread_mutex_unlock(&mut_rel_job);

	return 0;
}

void start_rel_job(){
	pthread_once(&init_once,module_load);
}
