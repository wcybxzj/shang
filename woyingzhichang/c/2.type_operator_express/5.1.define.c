#include <stdio.h>
//1.宏体要使用括号包裹
#define ADD 2+3
#define ADD_OK (2+3)

//2.宏中每个变量要使用括号包裹,减少你需要考虑里面优先级的问题
#define MAX(a, b) (a > b ? a : b)
#define MAX_BETTER(a, b) ((a) > (b) ? (a) : (b))

//3.带参宏中有 参数++ 的问题
//方法1:使用函数
//方法2:使用参数接受,非标准C,GUN C
//#define MAX_BEST(a, b) \
//	( {int A = a,B = b;  (A) > (B) ? (A) : (B);} )

#define MAX_BEST(a, b) \
	( {typeof(a) A = a,B = b;  ((A) > (B) ? (A) : (B));} )

int func(int a, int b);

int main(int argc, const char *argv[])
{
	printf("-------------宏体要用括号包裹---------------\n");
	int ret;
	ret = ADD * ADD; //期望得到25,但是11
	printf("%d\n", ret);
	ret = ADD_OK * ADD_OK; //OK
	printf("%d\n", ret);

	printf("-------------宏中的变量也要使用括号包裹-----\n");
	int a = 1,b = 2;
	ret = MAX(a, b+2);
	ret = MAX_BETTER(a, b+2);

	printf("------------ 宏中参数有++ ----------------\n");
	a = 5,b = 3;
	ret = MAX_BETTER(a++, b++);
	printf("ret %d\n", ret);
	printf("a %d, b %d\n", a ,b );

	printf("------------- 函数解决方法 ---------------\n");
	a = 5,b = 3;
	ret = func(a++, b++);
	printf("ret %d\n", ret);
	printf("a %d, b %d\n", a ,b );

	printf("------------ 宏中参数有++ ----------------\n");
	a = 5,b = 3;
	ret = MAX_BEST(a++, b++);
	printf("ret %d\n", ret);
	printf("a %d, b %d\n", a ,b );

	//-------------宏体要用括号包裹---------------
	//11
	//25
	//-------------宏中的变量也要使用括号包裹-----
	//------------ 宏中参数有++ ----------------
	//ret 6
	//a 7, b 4
	//------------- 函数解决方法 ---------------
	//in func a 5 b 3
	//ret 5
	//a 6, b 4
	//------------ 宏中参数有++ ----------------
	//ret 5
	//a 6, b 4

	return 0;
}

int func(int a, int b)
{
	printf("in func a %d b %d\n",a ,b);
	return a > b ? a : b;
}

