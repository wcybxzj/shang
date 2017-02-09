#include <stdio.h>

void func()
{
	static int a = 1;
	a++;
	printf("%p %d\n", &a, a);
}

int main(int argc, const char *argv[])
{
	func();
	func();
	func();
	return 0;
}
