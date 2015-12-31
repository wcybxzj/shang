#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	int count = 0;
	FILE *fp;
	if (argc < 2) {
		fprintf(stderr, "Uusage..\n");
		exit(1);
	}
	fp = fopen(argv[0], "r");

	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}

	while (fgetc(fp) != EOF) {
		count++;
	}
	printf("count %d\n", count);
	return 0;
}
