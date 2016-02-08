#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define LOOP_TIMES    10

int main(int argc, char *argv[])
{
	int i;
	int seed =123;
	for(i=0; i< LOOP_TIMES; i++) {
		printf("seed:%d\n", seed);
		printf("ret:%d\n", rand_r(&seed));
	}

	printf("\n");

	return 0;
}


