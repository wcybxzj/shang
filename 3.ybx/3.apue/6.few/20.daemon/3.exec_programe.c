#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	for (i = 0; i < 10; i++) {
		printf("exec %d\n", i);
	}
	return 0;
}
