#include <stdio.h>
int abc =123;
int main(int argc, const char *argv[])
{
	printf("%d\n", abc);

	{
		int abc = 111;
		printf("%d\n", abc);
	}

	return 0;
}
