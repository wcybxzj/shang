#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE 1024

int main(int argc, const char *argv[])
{
	int fd;
	fd = open("max.txt",O_WRONLY|O_CREAT, 0600);
	lseek(fd, 5LL*1024LL*1024LL*1024LL, SEEK_SET);
	write(fd,"",1);
	return 0;
}
