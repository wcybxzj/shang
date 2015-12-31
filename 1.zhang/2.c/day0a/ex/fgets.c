#include <stdio.h>
int main(int argc, const char *argv[])
{
	char str1[100];
	char *str = NULL;
	while ( fgets(str1, 100, stdin) ) {
		printf("%s",str1);
	}
	return 0;
}
