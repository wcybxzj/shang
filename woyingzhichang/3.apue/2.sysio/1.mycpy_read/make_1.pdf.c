#include <stdio.h>
int main(int argc, const char *argv[])
{
	FILE *fp;
	fp = fopen("1.pdf", "w");
	fseek(fp, 16L*1024*1024, SEEK_SET);
	fputc('\0',fp);
	fclose(fp);
	return 0;
}
