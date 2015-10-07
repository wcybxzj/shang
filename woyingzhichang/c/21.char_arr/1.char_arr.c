#include <stdio.h>
#define NUM 32
int main(int argc, const char *argv[])
{
	//1.初始化
	char str[NUM] = "aabbcc";
	char str1[NUM], str2[NUM];
	//char str[NUM] = {'a', 'b','c','\0'};
	//printf("%ld\n", sizeof(str));//32
	//printf("%ld\n", strlen(str));//32
	//printf("%s\n", str);

	//2.输入 输出

	//2.1
	//gets(str);
	//puts(str);

	//2.2
	// %s中不能有,空格/换行/回车
	// scanf存在内存越界问题
	scanf("%s%s%s", str, str1, str2);
	printf("%s\n%s\n%s\n", str, str1, str2);

	return 0;
}
