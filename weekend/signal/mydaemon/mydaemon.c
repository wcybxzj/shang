#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>

#define FNAME		"/tmp/out"

static FILE *fp;

int daemonize(void)
{
	pid_t pid;
	int fd;

	pid = fork();
	if(pid < 0)
	{
//		perror("fork()");
		syslog(LOG_ERR,"fork():%s",strerror(errno));		
		return -1;
	}

	if(pid > 0)		// parent
		exit(0);
	
	// child
			
	//close(0);
	//close(1);
	//close(2);
	
	fd = open("/dev/null",O_RDWR);
	if(fd < 0)
	{
//		perror("open()");
		syslog(LOG_WARNING,"open():%s",strerror(errno));
	}
	else
	{
		dup2(fd,0);
		dup2(fd,1);
		dup2(fd,2);
	}
	setsid();
	/*if error*/

	chdir("/");
//	umask(0);

	return 0;
}

static void daemon_exit(int s)
{
	closelog();
	fclose(fp);

	exit(0);
}

int main()
{
	int i;
	struct sigaction sa;

//	signal(SIGINT,daemon_exit);
//	signal(SIGTERM,daemon_exit);
//	signal(SIGQUIT,daemon_exit);


	sa.sa_handler = daemon_exit;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGINT);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sa.sa_flags = 0;
	sigaction(SIGINT,&sa,NULL);
	/*if error*/
	sigaction(SIGTERM,&sa,NULL);
	/*if error*/
	sigaction(SIGQUIT,&sa,NULL);
	/*if error*/

	

	
	openlog("mydaemon",LOG_PID,LOG_DAEMON);


	if(daemonize())
	{
		syslog(LOG_ERR,"daemonize() failed.");
		exit(1);
	}
	else
		syslog(LOG_INFO,"daemonize() successed.");	


	/*do sth*/
	
	fp = fopen(FNAME,"w");
	if(fp == NULL)
	{
//		perror("fopen()");
		syslog(LOG_ERR,"fopen():%s",strerror(errno));
		exit(1);
	}

	for(i = 0 ; ; i++)
	{
		fprintf(fp,"%d\n",i);
		fflush(fp);
		syslog(LOG_DEBUG,"%d was printed.",i);
		sleep(1);
	}


	exit(0);
}


