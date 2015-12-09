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

union {
	int a;
	double d;
	struct {
		int arr[10];
		float f;
	}c;
}test;

int main(int argc, const char *argv[])
{
	printf("%d\n", sizeof(a));//16
	printf("%d\n", sizeof(test));//44
	return 0;
}
