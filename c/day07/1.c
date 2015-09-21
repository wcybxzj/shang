#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	char *p;
	//char str1[]="abcdef";
	char *str1 = malloc(7);
	strcpy(str1, "abcdef");
	p = str1;

	printf("%s\n", p);
	*(p++) = 'x';
	printf("%s\n",str1);

	printf("==============\n");

	*(str1++) = 'y';
	str1--;
	printf("%s\n",str1);


	printf("--------------\n");

	char *str;
	str = malloc(10);
	str[0]='a';
	str[1]='b';
	str[2]='\0';
	//*str++='a';
	//*str++='b';
	//*str ='\0';
	printf("%s\n",str);
	return 0;
}
