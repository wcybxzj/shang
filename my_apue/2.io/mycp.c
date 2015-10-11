#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE 1024

int main(int argc, const char *argv[])
{
	int fd1, fd2;
	int n;
	char buf[BUFSIZE];
	if (argc<3) {
		printf("arg error\n");
		exit(1);
	}

	fd1 = open(argv[1],O_RDONLY);
	fd2 = open(argv[2],O_WRONLY|O_CREAT,0600);

	while ( n = read(fd1, buf, BUFSIZE)) {
		 write(fd2,buf,n);
	}

	return 0;
}
