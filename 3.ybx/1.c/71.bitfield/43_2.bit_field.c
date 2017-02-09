#include <stdio.h>
#include <stdlib.h>
/*
1:位域没有兼容性,在大端和小端中存储的顺序不同
2:百度:补码
3:有符号十进制数据的保存方法
	0:直接保存2进制
	正数:直接保存2进制
	负数:其值的绝对值->取反->加1
		如:-5的2进制保存格式是:
		5的二进制101, 取反->010, 加1->010+1=011
*/

union{
	struct {
		char a:1;//占1位
		char b:2;//占2位
		char c:1;//占1位
	}x;
	char y;//1个字节 ８位
}w;

int main(int argc, const char *argv[])
{
	printf("%d\n", sizeof(w));//1 
	printf("%d\n", sizeof(char));//1
	printf("%d\n", sizeof(int));//4
	printf("%d\n",sizeof(w.y));//1

	w.y= 1;
	//%d有符号整数
	printf("%d\n",w.x.a);//-1
	printf("%d\n",w.x.b);//0
	printf("%d\n",w.x.c);//0

	w.y = 4;
	printf("%d\n",w.x.b);//-2

	w.y = 6;
	printf("%d\n",w.x.b);//-1

	return 0;
}
