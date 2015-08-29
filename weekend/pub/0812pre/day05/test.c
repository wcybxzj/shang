#include <stdio.h>
#include <string.h>

char *itoa(char *ptr, int var);
int main(void)
{
	char str[32];
	char str2[32];
	int a = 123456;
	int i, j;

	itoa(str, a);

	for (i = strlen(str)-1, j=0; i >=0; i--, j++) {
		str2[j] = str[i];
	}
	str2[j] = '\0';

	printf("%s\n", str);//1234	
	printf("%s\n", str2);//1234	

	return 0;
}

char *itoa(char *ptr, int var)
{
	int i;

	for (i = 0; var > 0; i++) {
		ptr[i] = var % 10 + '0';
		var = var / 10;
	}
	ptr[i] = '\0';
	return ptr;
}
