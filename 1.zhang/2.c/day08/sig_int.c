#include <stdio.h>
#include <signal.h>

void sig_handler(int unuse)
{
	printf("你怎么还不死呢\n");
}

int main(void)
{
	signal(2, sig_handler);

	while (1) {
		printf("气死你\n");
		sleep(1);
	}

	return 0;
}
