#include <stdio.h>
#include <stdlib.h>

int myatoi(const char *p);
int main(void)
{
	printf("*******%d\n", myatoi("1234"));

	return 0;
}

int myatoi(const char *p)
{
	int num = 0;

	while (*p) {
		num = num*10 + (*p)-'0';
		p++;
	}	

	return num;
}

