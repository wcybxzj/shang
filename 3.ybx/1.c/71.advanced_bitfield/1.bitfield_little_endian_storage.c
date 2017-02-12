#include <stdio.h>

typedef struct {
	unsigned char a;//内存低地址
	unsigned char b;
	unsigned char c;
	unsigned char d;//内存高地址
} my_st1;

typedef struct  {
	unsigned char a;   //内存低地址
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
	unsigned char b4:1;
	unsigned char b5:1;
	unsigned char b6:1;
	unsigned char b7:1;
	unsigned char b8:1;
	unsigned char c;
	unsigned char d;  ;//内存高地址
} my_st2;

typedef struct {
	unsigned char a1:2;//内存低地址
	unsigned char a2:3;
	unsigned char a3:3;

	unsigned char b1:2;
	unsigned char b2:3;
	unsigned char b3:3;

	unsigned char c1:2;
	unsigned char c2:3;
	unsigned char c3:3;

	unsigned char d1:2;
	unsigned char d2:3;
	unsigned char d3:3;//内存高地址
} my_st3;


typedef union {
	unsigned int n;
	my_st1 var1;
	my_st2 var2;
	my_st3 var3;
}my_un;

int main(int argc, const char *argv[])
{
	my_un un;
	un.n = 0x12345678;

	//1.证明多字节类型中字节的存储是按照小端存储
	printf("%#x\n",un.var1.b);//0x56的二进制01010110
	printf("---------------------------------\n");

	//2.证明在1个字节中的8个位也是按照小端存储
	printf("%d",un.var2.b1); //0
	printf(" %d",un.var2.b2);//1
	printf(" %d",un.var2.b3);//1
	printf(" %d",un.var2.b4);//0
	printf(" %d",un.var2.b5);//1
	printf(" %d",un.var2.b6);//0
	printf(" %d",un.var2.b7);//1
	printf(" %d\n",un.var2.b8);//0
	printf("---------------------------------\n");

	//3.同2
	printf("%d\n", un.var3.b1);//2 10
	printf("%d\n", un.var3.b2);//5 101
	printf("%d\n", un.var3.b3);//2 010
	return 0;
}
