#include <stdio.h>
//http://blog.csdn.net/pony_maggie/article/details/37535577
//http://blog.csdn.net/jaylong35/article/details/6160736
void test1()
{
	//1.说明数字0xff是int
	printf("%d\n", sizeof(0xff));//4

	//2.
	if (0xff == 0x000000ff) {
		printf("equal\n");//equal
	}else{
		printf("not equal\n");
	}

	//3.
	if (0xff == 0x00ff) {
		printf("equal\n");//equal
	}else{
		printf("not equal\n");
	}
}

//1.变量如果是无符号类型根本不需要符号位扩展,只需要将高位填充为0
//0x83的二进制是10000011, unsigned char b
//强转成short就是00000000 10000011-->131
void test2()
{
	unsigned char b = 0x83;
	short s1 = (short)b;
	printf("%d\n", s1);//131
}

//第二个结果(s2)才是我们期望的:
//如果变量b是正数(这里是负数，因为char表示有符号数，0x83最高位是1,表示负数)，s1和s2的结果是相等的。

//知识点1.符号位扩展和零扩展
//这里从char到short, 在前面扩展bit位, 达到高精度数的长度。
//那么扩展时，是在前面补0还是补1呢？
//这里有个原则就是，有符号数扩展符号位，也就是1,无符号数扩展0。

//难点
//知识点2.符号位扩展的要求:
//条件1:变量b本身是个singed char
//条件2:10000011 符号位为1
//两个条件都成立时,在从计算机从低精度数向高精度数转换时要进行符号位扩展

//知识点3.参考tan_c 3.2.3整数数据的分布是从负数10进制如何存成2进制
//本例子现在是从负数的2进制求负数的10进制
void test3()
{
	//知识点4.char默认是signed
	//0x83的二进制是10000011
	//最左边是1说明是负数
	char b = 0x83;

	//知识点5.让一个让数从低精度类型转成高精度类型
	//通过2进制数求出10进制是多少,不做特殊处理
	//step1:先求b的二进制进行符号位扩展
	//10000011->11111111 10000011
	//step2:减少1
	//11111111 10000010
	//step3:取反
	//00000000 01111101
	//step4:转成10进制
	//125
	//step5:加上符号
	//-125
	short s1 = (short)b;

	//知识点6.让一个让数从低精度类型转成高精度类型
	//通过2进制数求出10进制是多少,如果负数强制转换成正数

	//知识点6.1.办法1:
	//通过让知识点3的条件2不成立,不进行符号位扩展
	//step1:
	//0xff其实是0x000000ff
	//b&0xff 
	//相当于 00000000 10000011 & 00000000 11111111
	//结果是:00000000 10000011 
	//step2:
	//现在符号位成0了无符号数扩展零不需要任何变动
	//step3:
	//一个正数的二进制存储就是其正数值
	//10000011--->131
	short s2 = (short)(b&0xff);

	//知识点6.2.办法2
	//通过让知识点3的条件1不成立,不进行符号位扩展
	//step1:
	//先用(unsigned char) b,直接把b类型改变成unsigned
	//step2:
	//(short)(unsigned char) b结果
	//00000000 10000011-->131

	short s3 = (short)(unsigned char) b;

	printf("s1 = %d\n", s1);//-125
	printf("s2 = %d\n", s2);//131
	printf("s3 = %d\n", s3);//131
}

//结果:notequal
//分析:char a是有符号, a的二进制符号位为1
//a == 0xff时候, a是signed char, 0xff是int
//a隐式转成int,需要符号位扩展
//a从11111111变成0xffffffff存储到内存中
//0xff在内存中是0x000000ff两者不等
void test4()
{
	char a = 0xff;
	if (a == 0xff) {
		printf("equal\n");
	}else{
		printf("not equal\n");
	}
}

//符号位扩展和零扩展
int main(void)
{
	//test1();
	//test2();
	test3();

	return 0;
} 
 
