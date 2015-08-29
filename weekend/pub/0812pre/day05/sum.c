#include <stdio.h>

int temp(int a);
int jiec_sum(int num);

int main(void)
{
	int var;

	printf("input:\n");
	scanf("%d", &var);

	printf("*******%d********\n", jiec_sum(var));

	return 0;
}

int jiec_sum(int num)
{
	int i;	
	int sum = 0;
	
	for (i = 1; i <= num; i++) {
		sum += temp(i);
	}

	return sum;
}

int temp(int a)
{
	int ret = 1;

	for (; a > 0; a--) {
		ret *= a;
	}

	return ret;
}

