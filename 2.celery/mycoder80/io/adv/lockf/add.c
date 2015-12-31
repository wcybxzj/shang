#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THRNUM		20	
#define FNAME		"/tmp/out"
#define BUFSIZE		1024

static void thr_add(void)
{
	int fd;
	FILE *fp;
	char buf[BUFSIZE];

	fp = fopen(FNAME,"r+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0);
	fgets(buf,BUFSIZE,fp);
	fseek(fp,0,SEEK_SET);
	sleep(1);//如果不加锁sleep增大冲突概率 让20个进程都取得1后都加1，重复加20个2,结果还是2
	fprintf(fp,"%d\n",atoi(buf)+1);
	fflush(fp);
	lockf(fd, F_ULOCK, 0);
	fclose(fp);
}

int main()
{
	int i;
	pid_t pid;
	for (i = 0; i < THRNUM; i++) {
		pid = fork();
		if (pid<0) {
			perror("fork()");
			exit(1);
		}
		if (pid==0) {
			thr_add();
			exit(1);
		}
	}

	for (i = 0; i < THRNUM; i++) {
		wait(NULL);
	}

	


	exit(0);
}
	

