#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	int j = 123;
	int *p = NULL;
	p = malloc(sizeof(int));
	*p = 11;
	printf("%d\n", *p);
	return 0;
}
