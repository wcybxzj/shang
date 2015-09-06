#include <stdio.h>

struct test{
	int arr[3];
	int var;
};

int main(void)
{
	struct test num = {{1,2,3}, 10};

	num.var;

	return 0;
}
