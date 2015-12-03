#include <stdio.h>

/*
	3! = 3 * 2 * 1
	3! = 3 * 2!
	2! = 2 * 1!

	0! =1
*/

int func(int n)
{
	if (n==0) {
		return 1;
	}
	return n * func(n-1);
}


int main(int argc, const char *argv[])
{
	int res;
	int n;
	scanf("%d", &n);
	res = func(n);
	printf("%d\n", res);
	return 0;
}
