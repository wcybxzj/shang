#include <stdio.h>

struct  st1{
	unsigned char a1:1;
	unsigned char a2:1;
	unsigned char a3:1;
	unsigned char a4:1;
	unsigned char a5:1;
	unsigned char a6:1;
	unsigned char a7:1;
	unsigned char a8:1;
};

struct  st2{
	char a1:1;
	char a2:1;
	char a3:1;
	char a4:1;
	char a5:1;
	char a6:1;
	char a7:1;
	char a8:1;
};

union un{
	char a;
	struct st1 b;
	struct st2 c;
};

//bit field和符号位扩展结合的代码
int main(int argc, const char *argv[])
{

	union un var;
	var.a = -1;//二进制11111111

	//1.unsigned char 转成 int
	printf("%d ",var.b.a1);//1
	printf("%d ",var.b.a2);//1
	printf("%d ",var.b.a3);//1
	printf("%d ",var.b.a4);//1
	printf("%d ",var.b.a5);//1
	printf("%d ",var.b.a6);//1
	printf("%d ",var.b.a7);//1
	printf("%d ",var.b.a8);//1

	//2.singed char 转成int,并且c.a1到c.a8符号位都是1
	//需要进行符号扩展

	//3.以c.a2为例:
	//从二进制取得十进制
	//step1:
	//a2是1个bit 值为二进制1
	//step2:
	//a2是signed并且符号位为1说明这个数是负数,
	//从低精度转换成高精度需要符号位扩展
	//11111111 11111111 11111111 11111111
	//step3:
	//减少1
	//11111111 11111111 11111111 11111110
	//step4:
	//取反得到绝对值
	//00000000 00000000 00000000 00000001-->十进制的1
	//step5:
	//加上负号
	//结果:-1
	printf("\n================\n");
	printf("%d ",var.c.a1);//-1
	printf("%d ",var.c.a2);//-1
	printf("%d ",var.c.a3);//-1
	printf("%d ",var.c.a4);//-1
	printf("%d ",var.c.a5);//-1
	printf("%d ",var.c.a6);//-1
	printf("%d ",var.c.a7);//-1
	printf("%d ",var.c.a8);//-1

	return 0;
	 
}
