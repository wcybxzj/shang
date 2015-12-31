#include <stdio.h>
#include <string.h>
int main(int argc, const char *argv[])
{
	char str[32]="abc\0def";

	printf("%ld\n", sizeof(str));
	printf("%ld\n", strlen(str));

	return 0;
}
