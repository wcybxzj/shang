#include <stdio.h>
int main(int argc, const char *argv[])
{
	struct st {
		char a;
		long d;
		char c;
		char e;
		char f;
	};

	struct st s;
	printf("%d\n", sizeof(s));

	printf("%d\n", sizeof(struct st));

	printf("=========================\n");

	struct st1{
		char a;
		short b;
		long c;
	};
	printf("%d\n", sizeof(struct st1));

	return 0;
}
