#include <stdio.h>
#include <string.h>

#define STRSIZE 3



int main(int argc, const char *argv[])
{
	char str[STRSIZE];

	//如果dst空间小于src
	//strcpy就会越界,strcpy设置好目标大小就没事
	//strcpy(str, "yangbingxi");
	//puts(str);

	char chararray[6];
	(void)strncpy(chararray, "abcdefgh", sizeof(chararray)-1);
	chararray[sizeof(chararray)-1] = '\0';
	//strlcpy(chararray, "abcdefgh", sizeof(chararray));
	printf("%s\n", chararray);

	return 0;
}
