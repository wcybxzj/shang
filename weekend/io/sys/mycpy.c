#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE		1024	

int main(int argc,char **argv)
{
	int sfd,dfd;
	char buf[BUFSIZE];
	int pos,len,ret;

	if(argc < 3)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

	sfd = open(argv[1],O_RDONLY);
	if(sfd < 0)
	{
		perror("open()");
		exit(1);
	}	

	dfd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0600);
	if(dfd < 0)
	{
		close(sfd);
		perror("open()");
		exit(1);
	}

	while(1)
	{
		len = read(sfd,buf,BUFSIZE);
		if(len < 0)
		{
			perror("read()");
			break;
		}
		if(len == 0)
			break;

		// len > 0

		pos = 0;

		while(len > 0)
		{
			ret = write(dfd,buf+pos,len);
			if(ret < 0)
			{
				perror("write()");
				break;
			}
			pos += ret;
			len -= ret;
		}
	}

	close(dfd);
	close(sfd);

	exit(0);
}



