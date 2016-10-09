#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 1024

int main(int argc, const char *argv[])
{
	char str[SIZE];
	FILE *fp;
	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	fp = fopen(argv[1],"r");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}
	
	while (fgets(str, SIZE, fp)) {
		printf("%s", str);
	}

	printf("ruid %d ,euid %d\n", getuid(), geteuid());
	fclose(fp);
	return 0;
}
