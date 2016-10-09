#include <stdio.h>
#include <stdlib.h>

//理解free 和 p=NULL
int main(int argc, const char *argv[])
{
	int *p = NULL;
	int *p1 = NULL;

	p = malloc(4);
	*p = 111;
	printf("%p-->%d\n", p, *p);
	free(p);
	//p = NULL;

	p1 = malloc(4);
	*p1 =222;
	printf("%p\n", p1);

	printf("%p\n", p);
	*p =112233;

	printf("%p-->%d\n", p1, *p1);
	printf("%p-->%d\n", p, *p);

	return 0;
}
