#include <stdio.h>
//位域  位段  unsigned int / int 

union {
	struct bit{
		unsigned int a : 2;
		unsigned int b : 2;
		unsigned int c : 2;
		unsigned int d : 2;
	}var;
	unsigned char e;
}test;

int main(void)
{
//	printf("sizeof(var) = %d\n", sizeof(var));

	test.e = 255;

	printf("test.var.a = %d, test.var.b = %d, test.var.c = %d,\
			test.var.d = %d\n", test.var.a, test.var.b, test.var.c, test.var.d);

	return 0;
}
