#include <stdio.h>

union stu{
	char a;
	int b;
}var;

int main(void)
{
	printf("sizeof(var) = %d\n", sizeof(var));
	printf("&var = %p\n", &var);
	printf("&var.a = %p\n", &var.a);
	printf("&var.b = %p\n", &var.b);

	var.b = 0x12345678;

	printf("var.a = %#x\n", var.a);

	return 0;
}
