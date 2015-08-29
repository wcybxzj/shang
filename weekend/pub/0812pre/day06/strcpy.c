#include <stdio.h>
#include <string.h>

char *mystrcpy(char *dest, const char *src);
int main(void)
{
	char str1[] = "loooooaaaaaaaaaaaaaaaaa";	
	char str2[] = "boys girls";

	printf("%s\n", mystrcpy(str1, str2));
	
	return 0;
}

char *mystrcpy(char *dest, const char *src)
{
	char *ret = dest;	

	while (*src) {
		*dest++ = *src++;
	}
	*dest = '\0';

	return ret;
}

