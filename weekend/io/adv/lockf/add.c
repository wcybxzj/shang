#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROCNUM		20	
#define FNAME		"/tmp/out"
#define SIZE		1024

static void func_add()
{
	FILE *fp;
	int fd;
	char buf[SIZE];

	fp = fopen(FNAME,"r+");	
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	fd = fileno(fp);
	/*if error*/
	
	lockf(fd,F_LOCK,0);

	fgets(buf,SIZE,fp);
	rewind(fp);
//	sleep(1);
	fprintf(fp,"%d\n",atoi(buf)+1);
	fflush(fp);

	lockf(fd,F_ULOCK,0);

	fclose(fp);
	return ;
}

int main()
{
	int i,err;	
	pid_t pid;

	for(i = 0; i < PROCNUM ;i++)
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

	for(i = 0; i < PROCNUM ;i++)
		wait(NULL);

	exit(0);
}


