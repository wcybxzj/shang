#include <stdio.h>

typedef int (*p_t)(int *);

int init(int *p)
{
	static int n = 1;	

	*p = n++;
	return 0;
}

int sum(int *p)
{
	static int sum = 0;		
	
	sum += *p;
	return sum;
}

int print(int *p)
{
	printf("%d ", *p);
}

int traval(int *arr, int num, p_t po)
{
	int i;
	int ret;

	for (i = 0; i < num; i++) {
		ret = po(arr+i);	
	}	
	return ret;
}

int main(void)
{
	int arr[5] = {};

	traval(arr, 5, init);	
	traval(arr, 5, print);
	printf("\n");
	printf("%d\n", traval(arr, 5, sum));

	return 0;
}
