#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void func(char *str1)
{
	char string[10];
	printf("%d\n", strlen(str1));
	if (strlen(str1)<=10) {
		strcpy(string, str1);
	}
	//printf("%s\n",string);
}
int main(int argc, const char *argv[])
{
	char *s ="abcaaaaaaaaaaaaaaaaaaaaaa";
	func(s);
	return 0;
}
