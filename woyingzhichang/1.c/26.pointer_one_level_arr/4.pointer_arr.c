#include <stdio.h>
int main(int argc, const char *argv[])
{
	int a[] = {5, 1, 7,2 ,8, 3};
	int y;
	int *p = &a[1];
	y = (*--p)++;
	printf("%d\n", y);
	printf("%d\n", a[0]);
	return 0;
}
