#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

//make 1.flen_stat
int main(int argc, const char *argv[])
{
	off_t len;
	if (argc < 2) {
		fprintf(stderr, "lack of arg!!\n");
		exit(1);
	}


	struct stat buf;
	if (stat(argv[1], &buf) <0) {
		perror("stat():");
		exit(1);
	}

	printf("st_size:%lld\n", buf.st_size);
	printf("st_blocks:%lld\n", buf.st_blocks);
	printf("st_blksize:%ld\n", buf.st_blksize);

	return 0;
}
