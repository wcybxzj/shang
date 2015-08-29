#include <stdio.h>

int myatoi(char *ptr);
int main(void)
{
	char *p = "12345";	
	
	printf("%d\n", myatoi(p));

	return 0;
}

int myatoi(char *ptr)
{
	int ret = 0;

	while (*ptr != '\0') {
		ret = ret*10 + *ptr-'0';
		ptr ++;
	}	

	return ret;
}

