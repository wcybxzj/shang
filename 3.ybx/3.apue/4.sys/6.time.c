#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


int main(int argc, const char *argv[])
{
	int i, n=0;
	time_t start, end;
	start = time(NULL);
	sleep(2);
	for (i = 0; i < 30000000; i++) {
		n++;
	}
	end = time(NULL);
	printf("%ld\n",start);
	printf("%ld\n", end);
	return 0;
}
