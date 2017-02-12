#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//这里会涉及3个知识点:
//问题1:
//小端存储在多字节类型中每个字节的存储顺序问题
//小端存储在同一个char中8个位的存储顺序问题
//
//问题2:
//符号位扩展的问题
//
//问题3:
//有符号int存储到内存的问题

//int 和char 默认都是signed
struct A
{
	int a:5;
	int b:3;
};

struct B
{
	int a:1;
	int b:7;
};

void test1()
{
	char str[100] = "0134324324afsadfsdlfjlsdjfl";
	struct A d;
	memcpy(&d, str, sizeof(struct A));
	printf("%d\n",d.a);//-16
	printf("%d\n",d.b);//1
}

void test2()
{
	char str[100] = "0134324324afsadfsdlfjlsdjfl";
	struct B d;
	memcpy(&d, str, sizeof(struct B));
	printf("%d\n",d.a);//0
	printf("%d\n",d.b);//24
}

//这个例子是C笔记1中有当时没弄懂
int main(void)
{
	//test1();
	test2();
	return 0;
}
