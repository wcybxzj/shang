#include <stdio.h>
int main(int argc, const char *argv[])
{
	int file_size = 0;
	FILE *fp;
	fp = fopen("/etc/services", "r");
	if (NULL == fp) {
		perror("fopen():");
	}
	int c;
	while ( (c=fgetc(fp)) != EOF ) {
		file_size++;
		//printf("%c",c);
	}
	printf("size is %d\n", file_size);
	return 0;
}
