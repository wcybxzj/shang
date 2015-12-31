#include <stdio.h>
#include <stdlib.h>

void func1(char *p)
{
	printf("%p\n", &p);//0x200
}

void func2(char **p)
{
	printf("%p\n", p);//0x100
	*p = "abbc";
}

int main(int argc, const char *argv[])
{
	char *str;
	printf("%p\n", &str);//0x100
	func1(str);

	func2(&str);
	printf("%s\n", str);

	return 0;
}
