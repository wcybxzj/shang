#include <stdio.h>
#include <string.h>

int main(void)
{
	char str[6] = {'h','e', 'l', 'l', 'o', '\0'};
	char str1[] = "hello";
	char *p;

//	gets(p);//错误的，段错误
//	gets(str);
	fgets(str, 6, stdin);

	puts(str);
	printf("strlen(str) = %d\n", strlen(str));
	printf("sizeof(str) = %d\n", sizeof(str));
//	putchar(getchar());
	
	p = str1;
	
	printf("strlen(p) = %d\n", strlen(p));
	printf("sizeof(p) = %d\n", sizeof(p));

	return 0;
}
