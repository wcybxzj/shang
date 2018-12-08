#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **args)
{
	char buffer[20];
	int fd = atoi(args[1]);
	memset(buffer,0,sizeof(buffer) );
	ssize_t bytes = read(fd,buffer,sizeof(buffer)-1);
	if(bytes < 0)
	{
		perror("7.exec: read fail:");
		return -1;
	}
	else
	{
		printf("7.exec: read %ld, %s\n",bytes,buffer);
	}
	return 0;
}
