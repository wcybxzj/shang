#include <stdio.h>
#define CON1(a, b) a##e##b
#define CON2(a, b) a##b##00

#define A "abc"
#define B "def"
#define CON(a, b) a##b
#define WRAP_CON(a, b) CON(a, b)

//ok
void func1()
{
	printf("%f\n", CON1(8.5, 2));
	printf("%d\n", CON2(12, 34));
}

//error
void func2()
{
	//char *str = CON(A, B);//char *str = AB;// AB不存在
	//char *str = WRAP_CON(A, B);// char *str = "abc""def";//语法错误
}

int main() {
	func1();
	func2();
	return 0;
}
