#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 1024

int main(int argc, const char *argv[])
{
	char str[SIZE];
	FILE *fp;
	fp = fopen("/etc/shadow","r");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}
	while (fgets(str, SIZE, fp)) {
		printf("%s", str);
	}
	fclose(fp);
	printf("ruid %d euid %d\n", getuid(), geteuid());
	return 0;
}
