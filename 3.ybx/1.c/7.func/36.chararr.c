#include <stdio.h>

char * mystrcpy(char *dest, const char *src){
	char *re = dest;
	if (dest != NULL && src != NULL)
		while ((*dest++ = *src++)!='\0');
	return re;
}

char * mystrncpy(char *dest, const char *src, size_t num){
	char *re = dest;
	int i;
	for (i = 0; i < num; i++) {
		if ( (*dest++ = *src++) == '\0' ) {
			break;
		}
	}
	*dest = '\0';
	return re;
}

int main(int argc, const char *argv[])
{
	char str1[] = "hello world";
	char str2[128]= {'\0'};
	char str3[128]= {'\0'};
	char *tmp = NULL;
	tmp = mystrcpy(str2, str1);
	printf("%s\n", tmp);

	tmp = mystrncpy(str3, str1, 3);
	printf("%s\n", tmp);
	return 0;
}
