#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	char *name = NULL;
	//char tmp[10000];
	char *tmp = NULL;
	name = tmpnam(tmp);
	printf("%s\n", tmp);
	printf("%s\n", name);
	return 0;
}
