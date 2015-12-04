#include <stdio.h>

int add(int a, int b){
	return a + b;
}

int main(int argc, const char *argv[])
{
	int ret;
	int a = 3, b = 5;
	int (*func_name)(int, int);

	//func_name = add;
	func_name = &add;

	//ret = func_name(a ,b);
	ret = (*func_name)(a ,b);

	//ret = add(a ,b);

	printf("%d\n", ret);

	return 0;
}
