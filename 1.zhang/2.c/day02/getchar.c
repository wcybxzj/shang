#include <stdio.h>

int main(void)
{
	int ch;

	while (1) {
		ch = getchar();
		putchar(ch);
		fflush(stdout);
		sleep(1);
	}	

	return 0;
}
