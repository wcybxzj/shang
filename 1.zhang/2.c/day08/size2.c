#include <stdio.h>

#pragma  pack(4)

struct tt{
	char a;
	long b;
	char c;
};

int main(void)
{
	printf("%d\n", sizeof(struct tt));

	return 0;
}
