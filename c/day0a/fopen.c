#include <stdio.h>

#define FLNAME  "exam.c"

int main(void)
{
	FILE *fp = NULL;
	int ch;

	fp = fopen(FLNAME, "r");

	while (1) {
		ch = fgetc(fp);	
		if (ch == EOF)
			break;
		fputc(ch, stdout);
		fflush(stdout);
		sleep(1);
	}

	fclose(fp);

	return 0;
}
