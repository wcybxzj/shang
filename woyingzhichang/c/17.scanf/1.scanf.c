#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	float f;
	char str[3];

	printf("请输入一个整型 一个浮点数 一个字符串\n");
	//知识点1:
	//默认分隔符位 空格 制表符 回车，并且可以是多个
	scanf("%d%f",&i ,&f);
	printf("i %d f %f\n", i, f);

	//知识点2:
	//scanf 获取字符串中间不能有空格

	//知识点3:
	//本来职能存2个字符和一个\0
	//但是scanf并不去判断长度输入多少都行
	scanf("%s", str);
	printf("%s\n", str);
	return 0;
}
