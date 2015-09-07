#include <stdio.h>

int my_strlen(char *str)
{
	int i= 0;
	while (*str++ != '\0') {
		i++;
	}
	return i;  
}

char * my_strcpy(char *src, char *dest){
	char *p = dest;
	while ( (*dest++ = *src++) != '\0') ;
	*dest = '\0';
	return p;
}

char * my_strcat(char *src, char *dest){
	char re[100] = {};
	char *p = re;
	char *p1 = re;
	while ((*p1++ = *src++));
	if (*p1=='\0') {
		*p1 ='x';
	}
	while ((*p1++ = *dest++));
	*p1= '\0';
	return p;
}

int main(int argc, const char *argv[])
{
	char *str = "abc";
	printf("%d\n", my_strlen(str));
	printf("====================\n");

	char dest[20]={};
	my_strcpy(str, dest);
	printf("%s\n", dest);
	printf("====================\n");

	char *str2 = "def";
	printf("%s\n", my_strcat(str, str2));

	return 0;
}
