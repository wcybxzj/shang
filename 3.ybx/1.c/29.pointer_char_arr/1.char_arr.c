#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	//情况1:
	//正常使用
	char str1[] = "I love China!";
	char *p = str1+7;
	puts(str1);//I love China
	puts(p);//China
	printf("---------------------------------------\n");


	//情况2:
	//字符指针和字符数组的区别
	char *str0 = "hello";//指针指向,字符串常量
	printf("%d %d\n", sizeof(str0), strlen(str0));//4, 5
	//str0指向的是字符常量,不能被覆盖只能改变指针的指向
	//strcpy(str0, "world");//段错误
	str0 = "haha";
	puts(str0);
	char *p0 = str0+1;
	puts(p0);//ok
	printf("---------------------------------------\n");


	//情况3:
	char str[] = "hello";
	printf("%d %d\n",sizeof(str) ,strlen(str));//6, 5
	//str是数组名是个常量，除非初始化否则不能出现在等号左边
	//str = "world";//error
	strcpy(str, "world");//把hello的每个字符一个个替换成world
	puts(str);

	return 0;
}
