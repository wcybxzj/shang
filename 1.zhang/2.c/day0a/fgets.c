#include <stdio.h>

#define FLNAME  "exam.c"
#define BUFSIZE	10

int main(void)
{
	FILE *fp = NULL;
	int ch;
	char buf[BUFSIZE] = {};

	fp = fopen(FLNAME, "r");

	while (1) {
		if (fgets(buf, BUFSIZE, fp) == NULL)
			break;
		fputs(buf, stdout);
	}

	fclose(fp);

	return 0;
}
