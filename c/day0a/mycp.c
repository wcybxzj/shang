#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 1

int main(int argc, char **argv)
{
	FILE *rp = NULL;
	FILE *wp = NULL;	
	char buf[BUFSIZE] = {};
	int cnt;

	if ((rp = fopen("1.txt", "r")) == NULL) {
		perror("fopen()");		
		exit(1);
	}

	if ((wp = fopen("2.txt", "w+")) == NULL) {
		fclose(rp);
		perror("fopen()");
		exit(1);
	}

	while ((cnt = fread(buf, 1, BUFSIZE, rp)) > 0) {
		fwrite(buf, 1, cnt, stdout);
		fwrite(buf, 1, cnt, wp);
		fflush(stdout);
		fflush(wp);
	}

	fclose(rp);
	fclose(wp);

	exit(0);
}
