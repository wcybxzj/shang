#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 10

int main(int argc, char **argv)
{
	FILE *rp = NULL;
	FILE *wp = NULL;	
	char buf[BUFSIZE] = {};
	int cnt;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s src dest\n", argv[0]);
		exit(1);
	}

	if ((rp = fopen(argv[1], "r")) == NULL) {
		perror("fopen()");		
		exit(1);
	}

	if ((wp = fopen(argv[2], "w")) == NULL) {
		fclose(rp);
		perror("fopen()");
		exit(1);
	}

	while ((cnt = fread(buf, 1, BUFSIZE, rp)) > 0) {
		fwrite(buf, 1, cnt, wp);
	}

	fclose(rp);
	fclose(wp);

	exit(0);
}
