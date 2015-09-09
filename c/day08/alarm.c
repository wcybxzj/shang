#include <stdio.h>
#include <signal.h>

void sig_handler(int s)
{
	alarm(1);
	printf("alarm is coming\n");
}

int main(void)
{
	signal(SIGALRM, sig_handler);
	alarm(1);

	while (1);
	return 0;
}
