#include <stdio.h>
#include <stdint.h>

//方法1:
//预备知识大端小端(看笔记)
void func1()
{
	//32位4个字节 0xff 正好1个字节,8位
	//12 34 56 78  每个是一个字节
	uint32_t i = 0x12345678;
	printf("%#x\n", i >> 16);//右移动2个字节, 1234
	printf("%#x\n", i);//0x12345678
	printf("%x\n", i & 0xffff);//0xffff最后2个字节全是1,  5678

	//1234  
	//加
	//5678
	//等于
	//68ac
	printf("%x\n", (i>>16)+(i&0xffff));
}

union {
	struct{
		uint16_t i;//2个字节
		uint16_t j;//2个字节
	}x;
	uint32_t y;//4个字节
}a;

//方法2:共用体嵌套结构体
void func2()
{
	a.y = 0x11223344;
	printf("%x\n",a.x.i);//3344 ,以为是小端口，数据的高位在地址的低位
	printf("%x\n",a.x.j);//1122
	printf("%#x\n",a.x.i+a.x.j);//4466
}

int main(int argc, const char *argv[])
{
	//题目:无符号32位数高16位和低16位相加
	func1();
	//func2();

	return 0;
}
