#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(int argc, const char *argv[])
{
	int i;
	for (i = 0; environ[i]!=NULL; i++) {
		puts(environ[i]);
	}
	return 0;
}
