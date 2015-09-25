#include <stdio.h>

void func()
{
	static int aabbcc =123;
	printf("%d\n",aabbcc);
}

int main(int argc, const char *argv[])
{
	func();
	static int i;
	static int j = 1;
	printf("%d\n", j);
	return 0;
}
