#include <stdio.h>
#include <string.h>

int mystrlen(const char *p);

int main(void)
{
	printf("mystrlen():%d\n", mystrlen("good afternoon"));
	printf("strlen():%d\n", strlen("good afternoon"));

	return 0;
}

int mystrlen(const char *p)
{
	int len = 0;	

	while (*p++)
		len++;
	return len;
}

