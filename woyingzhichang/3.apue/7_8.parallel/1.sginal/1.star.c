#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void int_handler(int s){
	write(1, "!",1);
}

int main(int argc, const char *argv[])
{
	int i;
	//signal(SIGINT, SIG_IGN);//忽略此信号
	signal(SIGINT, int_handler);

	for (i = 0; i < 10; i++) {
		ret = write(1, "*", 1);
		ret = sleep(3);
	}
	return 0;
}
