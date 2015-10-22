#include <stdio.h>
int main(int argc, const char *argv[])
{
	char *str ="abcdef";
	write(1, str+3, sizeof(str));
	return 0;
}
