#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void)
{
	//test1();
	test2();
	return 0;
}
