#include <stdio.h>
#include <signal.h>


int main(int argc, const char *argv[])
{
	int i, j;
	for (i = 0; i < 10000; i++) {
		for (j = 0; j < 5; j++) {
			write(1, "*", 1);
			sleep(1);
		}
		write(1, "\n", 1);
	}
	return 0;
}
