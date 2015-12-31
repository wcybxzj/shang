#include <stdio.h>
#define dPS int *
typedef int * tPS;

int main(int argc, const char *argv[])
{
	dPS p1,p2;
	tPS p3,p4;

	int a= 111;

	p1 =&a;
	p3 = &a;


	p2 =a;
	p4 = &a;


	return 0;
}
