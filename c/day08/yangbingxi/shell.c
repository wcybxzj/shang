#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	int i;
	int size = 300;
	char str[size];
	int len;
	char *p[3];
	int flag = 1;

	while (1) {
		printf("ybx$");
		fgets(str, size, stdin);
		if (strlen(str) == 1) {
			continue;
		}

		//printf("%s", str);
	}


	return 0;
}
