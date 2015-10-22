#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "mytbf.h"

#define CPS			10
#define BUFSIZE		1024	
#define BURST		1000

int main(int argc, const char *argv[])
{
	int sfd, dfd = 1;
	mytbf_t *tbf;

	if (argc < 2) {
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	tbf = mytbf_init(CPS, BURST);
	if (NULL == tbf) {
		fprintf(stderr, "mytbf init()\n");
	}


	return 0;
}
