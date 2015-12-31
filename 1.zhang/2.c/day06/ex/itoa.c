#include <stdio.h>
#include <stdlib.h>

char *string;
void itoa_r(int num)
{
	if (num==1) {
		printf("%d\n", num%10);
		return;
	}
	printf("%d\n", num%10);
	itoa_r(num/10);
}

int main(int argc, const char *argv[])
{
	int num = 12345;
	itoa_r(num);
	return 0;
}
