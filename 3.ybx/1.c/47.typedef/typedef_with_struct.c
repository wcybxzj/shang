#include <stdio.h>

typedef struct{
	int a;
	int b;
} ab;

typedef struct my_st{
	int c;
	int d;
}cd;

int main(int argc, const char *argv[])
{
	ab v1 = {11,22};
	cd v2 = {33,44};

	printf("%d %d\n", v1.a, v1.b);
	printf("%d %d\n", v2.c, v2.d);

	return 0;
}
