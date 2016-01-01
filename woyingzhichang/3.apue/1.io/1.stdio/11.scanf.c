#include <stdio.h>
int main(int argc, const char *argv[])
{
	int a, b;
	scanf("%d %d",&a ,&b);
	printf("%d %d\n", a, b);

	fscanf(stdin, "%d %d", &a, &b);
	printf("%d %d\n", a, b);

	//把字符串变成小字符串
	char *str = "yang-123";
	char s1[10];
	sscanf(str, "%s-%d", s1, &a);
	printf("%s -- %d\n",s1, a);

	return 0;
}
