#include <stdio.h>
int main(int argc, const char *argv[])
{
	int file_size = 0;
	FILE *fp;
	fp = fopen("/etc/services", "r");
	if (NULL == fp) {
		perror("fopen():");
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);

	printf("size is %d\n", file_size);
	return 0;
}
