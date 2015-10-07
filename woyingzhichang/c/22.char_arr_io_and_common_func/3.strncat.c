#include <stdio.h>
#include <string.h>

#define STRSIZE 10
int main(int argc, const char *argv[])
{
	char str[STRSIZE] = "hello";
	//strcat(str," yangbx");//Abort trap, 越界
	//printf("%s\n", str);

	strncat(str, " yangbx" , sizeof(str)-strlen(str)-1);
	printf("%s\n", str);

	return 0;
}
