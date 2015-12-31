#include <stdio.h>
#include <stdarg.h>

int sum(int n, ...)
{
	int ret = 0;
	va_list ap;

	va_start(ap, n);

	while (n > 0) {
		ret += va_arg(ap, int);
		n --;	
	}

	va_end(ap);//ap = NULL;   NULL == (void *)0

	return ret;
}

int main(void)
{
	printf("sum(3, 1,2,3) = %d\n", sum(3,1,2,3));	
	printf("sum(5, 1,2,3,4,5) = %d\n", sum(5, 1,2,3,4,5));	

	return 0;
}
