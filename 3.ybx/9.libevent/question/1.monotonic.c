#include <stdio.h>
#include <time.h>
int main(int argc, const char *argv[])
{
	struct timespec	ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		printf("支持\n");
	else
		printf("不支持\n");
	return 0;
}
