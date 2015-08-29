#include <stdio.h>
#include <string.h>

int mystrlen(char *ptr);
int sumstr(char *ptr);
int main(void)
{
	char *p = "abc";		

	printf("sizeof(p) = %d\n", sizeof(p));
	printf("mystrlen(p) = %d\n", mystrlen(p));
	printf("sumstr(p) = %d\n", sumstr(p));

	return 0;
}

int sumstr(char *ptr)
{
	int sum = 0;
	
	while (*ptr != '\0') {
		sum += *ptr;
		ptr++;
	}
	return sum;
}

int mystrlen(char *ptr)
{
	int len = 0;
	
	while ((*ptr) != '\0') {
		len++;
		//len = len + 1;
		ptr++;
	}
	return len;
}
