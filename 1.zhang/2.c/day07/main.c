#include <stdio.h>

int main(int argc, char *argv[], char *env[])
{
	int i;
	
	printf("argc == %d\n", argc);
	for (i = 0; argv[i]!=NULL; i++) {
		puts(argv[i]);
	}
	
	for (i = 0; env[i] != NULL; i++) {
		puts(env[i]);
	}

	return 0;
}
