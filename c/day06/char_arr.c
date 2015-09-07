#include <stdio.h>
#include <string.h>

int main(void)
{
	char str[6] = {'h','e', 'l', 'l', 'o', '\0'};
	char str1[] = "hello";
	char *p;

//  错误的，段错误,因为自动变量指针没有初始化,等于对非法内存进行操作
//	gets(p);
//	gets(str);
	fgets(str, 6, stdin);
	printf("%s", str);//fget如果没够指定数量会保存换行
	//puts(str);//等于printf("%s\n", str);

	printf("strlen(str) = %d\n", strlen(str));//1-5
	printf("sizeof(str) = %d\n", sizeof(str));//6   1个1个字节 
//	putchar(getchar());
	
	p = str1;
	printf("strlen(p) = %d\n", strlen(p));//5
	printf("sizeof(p) = %d\n", sizeof(p));//8

	return 0;
}
