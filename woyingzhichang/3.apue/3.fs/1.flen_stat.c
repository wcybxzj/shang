#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

static off_t flen(const char *fname)
{
	struct stat buf;
	if (stat(fname, &buf) <0) {
		perror("stat():");
		exit(1);
	}
	return buf.st_size;
}

//make 1.flen_stat
int main(int argc, const char *argv[])
{
	off_t len;
	if (argc < 2) {
		fprintf(stderr, "lack of arg!!\n");
		exit(1);
	}

	len = flen(argv[1]);
	printf("size:%lld\n", len);

	return 0;
}
