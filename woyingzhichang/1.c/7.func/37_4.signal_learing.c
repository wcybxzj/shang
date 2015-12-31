#include <stdio.h>

typedef void(*handler_t)(int);

handler_t func3(int, handler_t);

void func1(int a)
{
	printf("%d\n", a);
}

//必须按照第2种写法
//void (*)(int) func2(void (*funcp)(int))
void (*func2(void (*funcp)(int)))(int)
{
	return funcp;
}

int main(int argc, const char *argv[])
{
	void (*f)(int);
	f = func2(func1);
	f(777);

	printf("---------------------\n");


	f = func3(111, func1);
	f(888);

	return 0;
}

handler_t func3(int num, handler_t funcp)
{
	return funcp;
}
