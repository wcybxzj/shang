#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FNAME "/tmp/123.txt"

int main(int argc, const char *argv[])
{
	int fd;
	close(1);
	fd = open(FNAME, O_WRONLY|O_TRUNC|O_CREAT,0600);
	puts("ybx");
	return 0;
}
