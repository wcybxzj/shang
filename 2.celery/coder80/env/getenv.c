#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main()
{
	int i;

	puts(getenv("PWD"));

	exit(0);
}


