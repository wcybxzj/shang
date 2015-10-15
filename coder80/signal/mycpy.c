#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE		1024

int main(int argc,char **argv)
{
	int sfd,dfd;
	char buf[BUFSIZE];	
	int len,ret,pos;

	if(argc < 3)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

go:
	sfd = open(argv[1],O_RDONLY);
	if(sfd < 0)
	{
		if (errno==EINTR) {
			goto go;
		}
		perror("open()");
		exit(1);
	}

go1:
	dfd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0600);	
	if(dfd < 0)
	{
		if (errno==EINTR) {
			goto go1;
		}
		close(sfd);
		perror("open()");
		exit(1);
	}

	while(1)
	{
go2:
		len = read(sfd,buf,BUFSIZE);
		if(len < 0)
		{
			if (errno==ENINTR) {
				goto go2;
			}
			perror("read()");
			break;
		}
		
		if(len == 0)
			break;
		
		//len > 0

		pos = 0;

		while(len > 0)
		{
go3:
			ret = write(dfd,buf+pos,len);
			if(ret < 0)
			{
				if (errno==EINTR) {
					goto go3;
				}
				perror("write()");
				break;
			}
			len -= ret;
			pos += ret;
		}
	}
	
	close(dfd);
	close(sfd);

	exit(0);
}

