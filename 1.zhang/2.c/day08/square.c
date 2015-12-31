#include <stdio.h>

#define SQUREAR(m) (m)*(m)

#define SWAP(a, b) \
		do {\
			typeof(a) tmp;\
			tmp = a;\
			a = b;\
			b = tmp;\
		}while (0)

int main(void)
{
	printf("%d\n", SQUREAR(5+1));
	int a = 10;
	int b = 20;

	printf("a = %d, b = %d\n", a, b);
	SWAP(a, b);
	printf("a = %d, b = %d\n", a, b);

	return 0;
}
