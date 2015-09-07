#include <stdio.h>

int function(void)
{
	static int ret = 1;
	//只初始化一次

	ret ++;
	return ret;
}

int main(void)
{
	printf("%d\n", function());
	printf("%d\n", function());
	printf("%d\n", function());
	printf("%d\n", function());
	printf("%d\n", function());
	printf("%d\n", function());
	printf("%d\n", function());

	return 0;
}
