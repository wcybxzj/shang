#include <stdio.h>
int main(int argc, const char *argv[])
{
	//输入超过1个字符就会越界
	char str[1];
	//gets(str);
	//printf("%s\n", str);

	//fgets停止2种条件条件:
	//1.读取到size-1,size写为\0
	//2.读取到\n,然后\n后\0

	//加入输入a回车
	char str1[100];
	fgets(str, 2, stdin);//只能读1个字符
	printf("%s", str);//a\0
	printf("----------------\n");
	fgets(str, 2, stdin);//还剩下一个\n
	printf("%s", str);//\n\0
	return 0;
}
