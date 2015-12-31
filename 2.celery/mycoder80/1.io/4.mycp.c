#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	FILE *fp  = fopen("index.txt", "r");
	int c;
	while ( (c = getc(fp)) ) {
		if (c==-1) {
			break;
		}
		printf("%d\n", c);
	}

	return 0;
}
