#include <stdio.h>

struct te_st {
	char a;
	short b;
}__attribute__((packed));

int main(void)
{
	printf("sizeof(struct te_st) = %d\n", sizeof(struct te_st));

	return 0;
}
