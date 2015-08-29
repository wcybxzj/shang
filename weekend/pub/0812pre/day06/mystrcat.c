#include <stdio.h>

char *mystrcat(char *dest, const char *src);

int main(void)
{
	char str1[32] = "good ";
	char str2[] = "morning";

	printf("%s\n", mystrcat(str1, str2));

	return 0;
}

char *mystrcat(char *dest, const char *src)
{
	char *ret = dest;
	
	/*
	 	while (*dest != '\0') {
			dest ++;
		}
	 
	 */
	while (*dest++);

	dest --;
 
	/*
	 	while (*src != '\0') {
			*dest = *src;
			src++;
			dest++;
		}
	 
	 */
	while (*src) {
		*dest++ = *src++;
	}
	*dest = '\0';

	return ret;
}

