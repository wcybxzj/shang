#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	int i;
	int a,b;

	a = atoi(argv[1]);
	b = atoi(argv[3]);
	printf("%d\n",a);
	printf("%d\n",b);
	printf("%s\n", argv[2]);

	if (strcmp(argv[2],"+") == 0) {
		printf("%d\n", a+b);
	}else if (strcmp(argv[2],"-") == 0) {
		printf("%d\n", a-b);
	}

	return 0;
}
