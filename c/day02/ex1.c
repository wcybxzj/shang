#include <stdio.h>

int main(int argc, const char *argv[])
{
	int sum = 0;
	long int cheng =1;
	int a=10;
	int i;
	for (i = 1; i <= a; i++) {
		sum+=i;
	}
	printf("sum is %d\n", sum);

	for (i = 1; i <=a; i++) {
		cheng*=i;
	}
	printf("cheng is %lld\n", cheng);
	printf("cheng is %d\n", cheng);

	return 0;
}
