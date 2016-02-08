#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int s)
{
	printf("%d\n", s);
}

int main(int argc, const char *argv[])
{
	signal(SIGTERM, sig_handler);//可以执行
	signal(SIGKILL, sig_handler);//根本不会响应直接杀死

	while (1) {
		sleep(1);
	}
	return 0;
}
