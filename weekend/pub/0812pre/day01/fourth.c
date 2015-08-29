#include <stdio.h>

int main(void)
{
	int ret;

	ret = printf("goodgirl\n\0");

	printf("ret = %d\n", ret);	

	return 0;
}
