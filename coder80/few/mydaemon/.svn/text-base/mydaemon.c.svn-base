#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#define FNAME	"/tmp/out"

static int daemonize(void)
{
	int fd;
	pid_t pid;
	
	pid = fork();
	if(pid < 0)
	{
		syslog(LOG_ERR,"fork():%s",strerror(errno));
		return -1;
	}
	if(pid > 0)	// parent
		exit(0);
		
	// child
	
	//dup

//	close(0);
//	close(1);
//	close(2);

	fd = open("/dev/null",O_RDWR);
	if(fd < 0)
	{
		syslog(LOG_WARNING,"open():%s",strerror(errno));
	}
	else
	{
		dup2(fd,0);		
		dup2(fd,1);		
		dup2(fd,2);		
		if(fd > 2)
			close(fd);
	}
	
	setsid();

	chdir("/");
	umask(0);

	return 0;
}

int main()
{
	FILE *fp;
	int i;
	
	openlog("mydaemon",LOG_PID,LOG_DAEMON);

	if(daemonize())
	{
		syslog(LOG_ERR,"daemonize() failed.");
		exit(1);
	}
	else
	{
		syslog(LOG_INFO,"daemonize() successed.");
	}

	/*do sth*/

	fp = fopen(FNAME,"w");
	if(fp == NULL)
	{
		syslog(LOG_ERR,"fopen():%s",strerror(errno));
		exit(1);
	}

	syslog(LOG_DEBUG,"%s was opened",FNAME);	

	for(i = 0 ; ; i++)
	{
		fprintf(fp,"%d\n",i);
		fflush(fp);
		syslog(LOG_DEBUG,"%d was printed.",i);
		sleep(1);
	}

	fclose(fp);
	closelog();

	exit(0);
}


