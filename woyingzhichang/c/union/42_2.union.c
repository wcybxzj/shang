#include <stdio.h>
#include <stdlib.h>

struct {
	int i;
	char ch;
	union{
		int a;
		char ch;
	}un;
	float f;
}a;

int main(int argc, const char *argv[])
{
	printf("%d\n", sizeof(a));//16
	return 0;
}
