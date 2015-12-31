#include <stdio.h>
int main(int argc, const char *argv[])
{
	int c;
	while ((c=getchar()) != EOF) {
		sleep(1);
		putchar(c);
		fflush(stdout);
	}
}
