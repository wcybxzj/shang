#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEMSIZE		1024

int main()
{
	char *str;		
	pid_t pid;
	int shmid;

	shmid = shmget(IPC_PRIVATE,MEMSIZE,0600);
	if(shmid < 0)
	{
		perror("shmid");
		exit(1);
	}

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	
	if(pid == 0)	// child write
	{
		str = shmat(shmid,NULL,0);
		if(str == (void *) -1)
		{
			perror("shmat()");
			exit(1);
		}
		strcpy(str,"Hello!");
		shmdt(str);			
		exit(0);
	}
	else			// parent read
	{
		wait(NULL);
		str = shmat(shmid,NULL,0);
        if(str == (void *) -1)
        {
            perror("shmat()");
            exit(1);
        }
		puts(str);
		shmdt(str);
		shmctl(shmid,IPC_RMID,NULL);
        exit(0);
	}


	exit(0);
}



