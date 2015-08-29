#include <stdio.h>

#pragma pack(1)

struct te2_st {
	char a;
	long d;
	int b;
	char c;
};

int main(void)
{
	printf("sizeof(struct te2_st)  = %d\n", sizeof(struct te2_st));	

	return 0;
}
