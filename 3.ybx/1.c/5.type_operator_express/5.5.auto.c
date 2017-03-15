#include <stdio.h>
//本来局部变量都是auto，a应该是随机值但是测不到随机值
//./5.5.auto 
//0
void func1()
{
	int a;
	printf("%d\n", a);
}

//增加几个局部变量来查看随机值
//./5.5.auto 
//60
//4195696
//0
void func2()
{
	int a;
	int b;
	int c;
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);
}
int main(int argc, const char *argv[])
{
	//func1();
	func2();
	return 0;
}
