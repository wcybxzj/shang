#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	for (i = 0; argv[i] != NULL ; i++) {
		puts(argv[i]);
	}
	return 0;
}
