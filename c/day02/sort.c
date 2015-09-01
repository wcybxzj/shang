#include <stdio.h>

int main(void)
{
	int a, b, c;

	printf("input three numbers:\n");
	scanf("%d %d %d", &a, &b, &c);

	if (a > b) {
		a = a ^ b; // tmp = a;
		b = a ^ b; // a = b;
		a = a ^ b; // b = tmp;
	}
	if (b > c) {
		if (a > c) {
			a = a ^ c;
			c = a ^ c;
			a = a ^ c;	
		} 

		b = b ^ c;
		c = b ^ c;
		b = b ^ c;
	}

	printf("%d %d %d\n", a, b, c);

	return 0;
}
