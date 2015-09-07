#include <stdio.h>

int main(void)
{
	char arr[] = "hello";
	const char *p = arr;
	char const *q = arr;
	char *const r = arr;

	printf("%p\n", p);
	p++;
	printf("%p\n", p);

//	*p = 'o';错误的，空间只读

//	*q = 'a';错误的，同上

	*r = 'm';
//	r ++;变量只读
	printf("%s\n", r);

	return 0;
}
