#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SIZE 1024
int main(int argc, const char *argv[])
{
	char str[SIZE];
	char *tmp;
	char *p;
	while (1) {
		fgets(str, SIZE, stdin);
		tmp = str;
		while ((p = strsep(&tmp, " "))!=NULL) {
			printf("%s\n",p);
		}
	}

	return 0;
}
