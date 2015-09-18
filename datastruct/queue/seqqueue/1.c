#include <stdio.h>
int main(int argc, const char *argv[])
{
	char *str;
	printf("%d\n",sizeof(str));
	char s[0];
	printf("%d\n",sizeof(s));
	return 0;
}
