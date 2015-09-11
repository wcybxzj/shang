#include <stdio.h>
#include <stdlib.h>

#define BUF 1

int main(int argc, const char *argv[])
{
	FILE *fp1;
	FILE *fp2;
	fp1 = fopen(argv[1],"r");
	if (argv[2]) {
		fp2 = fopen(argv[2],"w+");
	}
	int num = 0;

	char buf[BUF];
	while (num = fread(buf, 1, BUF, fp1)) {
		if (argv[2]) {
			fwrite(buf,1, num, fp2);
		}
		fwrite(buf,1, num, stdout);
		fflush(stdout);
		sleep(1);
	}

	fclose(fp1);
	fclose(fp2);

	return 0;
}
