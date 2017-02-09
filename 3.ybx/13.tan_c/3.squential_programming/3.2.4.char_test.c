#include <stdio.h>

//分析:c是如何打印-1的
//1.存储:
//255的存进去的是2进制是11111111

//2.打印:
//打印时候按照%d signed int打印
//由于char默认是signed,左第一位是符号位, 1代表是负数
//负数从二进制存储转成10进制数的过程
//11111111先减1成为11111110
//取反得00000001也就是1
//因为是负数结果为-1
int main(int argc, const char *argv[])
{
	char c = 255;//char 默认是 signed
	printf("%d\n",c);//-1

	unsigned char c1 =255;
	printf("%d\n",c1);//255
	return 0;
}
