#include <stdio.h>

int * func()
{
	static int a;//a 在静态区
	a =123;
	return &a;
}

//char *func2()
//{
//	char str[10] = "abc";//str在栈空间
//	return str;//错误　
//}

char *func3(){
	char *str = "aaa";//str 在静态区
	return str;
}

int main(int argc, const char *argv[])
{
	int *tmp;
	tmp = func();
	printf("%d\n", *tmp);

	char *str;
	str = func3();
	printf("%s\n", str);
	return 0;
}
