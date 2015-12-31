#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 1024

int main(int argc, const char *argv[])
{
	FILE *fp1, *fp2;
	char str[SIZE];
	if (argc < 3) {
		printf("lack of argv\n");
		exit(1);
	}

	fp1 = fopen(argv[1],"r");
	if (NULL==fp1) {
		perror("fopen():");
		exit(2);
	}

	fp2 = fopen(argv[2],"w");

	while (1) {
		if (NULL == fgets(str, SIZE, fp1)) {
			break;
		}
		fputs(str, fp2);
	}

	fclose(fp1);
	fclose(fp2);
	return 0;
}
