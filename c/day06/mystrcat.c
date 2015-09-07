#include <stdio.h>

#define SIZE 32

char *mystrcat(char *dest, const char *src);
int main(void)
{
	char dest[SIZE] = "it is too ";
	char src[] = "too crazy";

	printf("%s\n", mystrcat(dest, src));

	return 0;
}

char *mystrcat(char *dest, const char *src)
{
	char *ret = dest;
		
	while (*dest++)
		;
	dest --;
	while (*src) 
		*dest++ = *src++;
	*dest = '\0';

	return ret;
}
