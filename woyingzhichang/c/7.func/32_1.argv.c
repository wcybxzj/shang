#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	for (i = 0;  argv[i] != NULL ; i++) {
		printf("%s\n", argv[i]);
	}
	return 0;
}
