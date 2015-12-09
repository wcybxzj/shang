#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void func(int **p, int num)
{
	*p = malloc(sizeof(int)*num);
	if (NULL == *p) {
		printf("%s\n", strerror(errno));
		exit(1);
	}
	return;
}

void* func1(int num){
	int *p = NULL;
	p = malloc(sizeof(int));
	return p;
}
int main(int argc, const char *argv[])
{
	int *p = NULL;;
	int num = 1;
	func(&p ,1);
	*p = 123;
	printf("%d\n", *p);
	free(p);

	int *p1 = NULL;
	p1 = p;
	*p1 =456;
	printf("%d\n", *p1);
	free(p1);

	return 0;
}
