#include <stdio.h>

int func()
{
	printf("working\n");
	return 1;
}

int main(int argc, const char *argv[])
{
	int a, b;
	a = 1;
	b =20;
	printf("%d\n",a++, b=++a, func());
	printf("%d, %d\n",a ,b);//3,2
	return 0;
}
