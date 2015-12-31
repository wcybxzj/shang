#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//diff 1.txt 2.txt
int main(int argc, const char *argv[])
{
	FILE *fp1, *fp2;
	char tmp;
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

	while ((tmp = fgetc(fp1))!=EOF) {
		fputc(tmp, fp2);
	}

	fclose(fp1);
	fclose(fp2);
	return 0;
}
