#include <stdio.h>

typedef int FUNC(int);
FUNC abc;

int main(int argc, const char *argv[])
{
	abc(123);
	return 0;
}

int abc(int a)
{
	printf("%d\n", a);
}
