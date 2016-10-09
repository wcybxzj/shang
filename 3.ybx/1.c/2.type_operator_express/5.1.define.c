#include <stdio.h>
//1.宏体要使用括号包裹
#define ADD 2+3
#define ADD_OK (2+3)

//2.宏中每个变量要使用括号包裹,减少你需要考虑里面优先级的问题
#define MAX(a, b) (a > b ? a : b)
#define MAX_BETTER(a, b) ((a) > (b) ? (a) : (b))

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
	printf("ret %d\n", ret);

	a = 1,b = 2;
	ret = MAX_BETTER(a, b+2);
	printf("ret %d\n", ret);

	printf("------------ 宏中参数有++ ----------------\n");
	a = 5,b = 3;
	ret = MAX(a++, b++);
	printf("ret %d\n", ret);
	printf("a %d, b %d\n", a ,b );

	a = 5,b = 3;
	ret = MAX_BETTER(a++, b++);
	printf("ret %d\n", ret);
	printf("a %d, b %d\n", a ,b );


	return 0;
}
