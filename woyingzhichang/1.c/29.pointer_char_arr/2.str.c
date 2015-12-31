#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void normal_use()
{
	char name[20] = "error";
	char  *str = "this is fine";

	char *str1;
	str1 = "this is ok";
	str1 = "nice person";
	//strcpy(str1, "aaaa");//段错误,指针指向的是字符常量不能被修改
	str1 = str1+5;
	printf("%s\n", str1);//person
	
	char name1[20];
	//name是数组名是个常量,除非初始化否则不能出现在等号左边
	//name = "abcdef";//error
	strcpy(name, "abc");
	printf("%s\n", name);
}

void func_strcpy()
{
	//例1:char *str　主要支持直接改变指针地址,要想支持strcpy 必须借助malloc
	char *str;
	//strcpy(str, "abc");//error 内存地址非法

	str = malloc(10);
	strcpy(str, "nice");//ok
	printf("%s\n", str);
	strcpy(str, "boy");//ok
	printf("%s\n", str);

	//error 指针指向的内容不能修改,是字符串常量
	char *str2 = "string constants";
	//strcpy(str2, "aaa");//error

	printf("--------------------------------\n");

	//例2:char name[] 主要支持strcpy
	char str3[10] = {'a', 'b', 'c', '\0'};
	strcpy(str3, "aabbcc");
	printf("%s\n", str3);

	char str4[20] = "still change";
	strcpy(str4, "can");
	printf("%s\n", str4);
}

void func_scanf()
{
 	//scanf中 char*str char name[]的区别
	char *str;
	//scanf("%s", str);//段错误, 非法内存的操作
	str = malloc(20);
	scanf("%s", str);
	scanf("%s", str);
	printf("%s\n",str);
	strcpy(str, "haha");
	printf("%s\n",str);

	//char name[10];
	//scanf("%s", name);//很危险因为无法判断输入的大小
	//printf("%s\n", name);
}

int main(int argc, const char *argv[])
{
	//normal_use();
	func_strcpy();
	return 0;
}
