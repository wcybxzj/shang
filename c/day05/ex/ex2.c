#include <stdio.h>

int set(int num, int n)
{
	 return num|1U<<(n-1);
}

int clear(int num, int n)
{
	 return num & ~(1U<<(n-1));
}


int main(int argc, const char *argv[])
{
	//1100
	int a = 12;
	int tmp;
	tmp = set(a,2);
	printf("%d\n", tmp);

	tmp = clear(tmp,2);
	printf("%d\n", tmp);

	return 0;
}
