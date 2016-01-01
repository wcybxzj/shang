#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 1024
int main(int argc, const char *argv[])
{
	int ret = 0;
	FILE *fp1, *fp2;
	char str[1024];
	fp1 = fopen("1.txt", "r");
	fp2 = fopen("2.txt", "w");

	while ((ret = fread(str, 1, MAXSIZE, fp1)) > 0) {
		fwrite(str,1, ret, fp2);
	}

	return 0;
}
