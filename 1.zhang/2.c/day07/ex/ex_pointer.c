#include <stdio.h>
int main(int argc, const char *argv[])
{
	int arr[] = {11,22,33};
	int *p = arr;
	printf("%d\n", *(p++));//11
	printf("%d\n", *(p++));//22

	return 0;
}
