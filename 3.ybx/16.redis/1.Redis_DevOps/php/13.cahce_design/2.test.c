#include <stdio.h>
int main(int argc, const char *argv[])
{
	char *str ="abc";
	printf("%c\n", *str++);//a
	printf("%c\n", *str);//b
	return 0;
}
