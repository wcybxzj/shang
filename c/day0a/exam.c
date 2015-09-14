#include <stdio.h>
#include <string.h>

struct {
	int a:5;
	int b:3;
}d;

int main(void)
{
	char str[] = "0123456abcdefghijk";
	
	memcpy(&d, str, sizeof(d));	

	printf("d.a = %d, d.b = %d\n", d.a, d.b);
	return 0;
}
