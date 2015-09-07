#include <stdio.h>
#include <stdarg.h>

void miniprintf(const char *p, ...)
{
	va_list ap;				

	va_start(ap, p);
	while (*p) {
		switch (*p) {
			case '%':
				switch (*(++p)) {
					case 'd'://下面的顺序不能变
						printf("%d", va_arg(ap, int));
						break;
					case 'c':
						printf("%c", va_arg(ap, int));
						break;
					case 's':
						printf("%s", va_arg(ap, char *));
						break;
					case 'f':
						printf("%f", va_arg(ap, double));
				}
				break;
			default:
				printf("%c", *p);
				break;
		}	
		p++;
	}
	va_end(ap);
}

int main(void)
{
	miniprintf("the result is %d and %f and %c, you are so %s\n", 250, 255.5, 'a', "great");

	return 0;
}

