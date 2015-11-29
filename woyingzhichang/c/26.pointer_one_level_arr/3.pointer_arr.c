#include <stdio.h>
int main(int argc, const char *argv[])
{
	int *p = (int [3]){1, 2, 3};
	int i;

	for (i = 0; i < 3; i++) {
		printf("%p --> %d\n",&p[i] ,p[i]);
	}

	return 0;
}
