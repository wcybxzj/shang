#include <stdio.h>

#define SIZE 32

char *mystrcpy(char *dest, const char *src);

int main(void)
{
	char src[] = "haha";
	char dest[SIZE] = "i am so hurgry";

	printf("%s\n", mystrcpy(dest, src));

	return 0;
}

char *mystrcpy(char *dest, const char *src)
{
	char *ret = dest;
	
	while (*src) 
		*dest++ = *src++;
	*dest = '\0';

	return ret;	
}

