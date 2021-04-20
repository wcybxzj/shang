#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

int main(int argc, char *argv[])
{

	sleep(2);

	if (argc>1) {
		printf("%s\n",argv[1]);
	}
	return 0;
}
