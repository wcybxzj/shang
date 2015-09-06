#include <stdio.h>

int my_sizeof(){
	int cnt = 0;
	unsigned int tmp = ~123u;
	printf("%u\n", tmp);
	while (tmp >> cnt++ != 0x1);
	return cnt;
}

int binary_print(int num)
{
	int i;
	for (i = my_sizeof(); i>=0;i--) {
		(num >> i) & 0x1 ? putchar('1'):putchar('0');
		if (i%8 == 0) {
			printf(" \n");
		}
	}
	printf("\n");
	return 0;
}

int main(int argc, const char *argv[])
{
	unsigned int var;

	//printf("input:\n");
	//scanf("%d", &var);
	binary_print(125);

	return 0;
}
