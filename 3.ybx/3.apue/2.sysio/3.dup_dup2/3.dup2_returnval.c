#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int fd = open("/tmp/1.txt", O_CREAT|O_TRUNC|O_RDWR);
	int back_fd = dup2(fd, 1);
	printf("%d\n", back_fd);
	return 0;
}
