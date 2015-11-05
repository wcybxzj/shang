#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define PROCNUM		20	
#define FNAME		"/tmp/out"
#define BUFSIZE		1024

static int semid;

static void P(void)
{
	struct sembuf op;
	
	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg = 0;
	while(semop(semid,&op,1) < 0)
	{
		if(errno != EINTR)
		{
			perror("semop()");
			exit(1);
		}
	}

}
static void V(void)
{
	struct sembuf op;

    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    while(semop(semid,&op,1) < 0)
    {
        if(errno != EINTR)
        {
            perror("semop()");
            exit(1);
        }
    }
}

static void func_add(void)
{
	FILE *fp;
	char buf[BUFSIZE];
	int fd;

	fp = fopen(FNAME,"r+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	P();	
	fgets(buf,BUFSIZE,fp);
	/*if error*/
	fseek(fp,0,SEEK_SET);
//	sleep(1);
	fprintf(fp,"%d\n",atoi(buf)+1);
	fflush(fp);
	V();

	fclose(fp);
	return ;
}

int main()
{//semarr[1] -> value=1 -> mut
	int err,i;	
	pid_t pid;

	semid = semget(IPC_PRIVATE,1,0600);
	if(semid < 0)
	{
		perror("semget()");
		exit(1);
	}

	if(semctl(semid,0,SETVAL,1) < 0)
	{
		perror("semctl()");
		exit(1);
	}

	for(i = 0 ; i < PROCNUM; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		if(pid == 0)
		{
			func_add();
			exit(0);
		}
	}

	for(i = 0 ; i < PROCNUM; i++)
		wait(NULL);

	semctl(semid,0,IPC_RMID);

	exit(0);
}
	

