#include <stdio.h>

union test_un{
	int i;
	float f;
	double d;
	char ch;
}a = {
	.i=123
};


int main(int argc, const char *argv[])
{
	union test_un *p = &a;
	union test_un arr[3];

	printf("%d\n", sizeof(a));//8 当前最大的值是double
	printf("%d\n", a.i);//123
	printf("%f\n", a.f);//00000 别以为能是什么123.000

	printf("----------------------------------\n");

	printf("%d\n", p->i);

	return 0;
}
