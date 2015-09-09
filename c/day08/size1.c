#include <stdio.h>

struct test_st{
	char a;
	int b;
	short c;
}__attribute__((packed));

int main(void)
{
	struct test_st var;
	printf("sizeof(var) = %d\n", sizeof(var));

	return 0;
}
