#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define FNAM "/etc/services"

FILE *fp;

/*
发送Ctrl+c和ctrl+ \ 报错fclose相当于free,不能对一段空间free2次
./a.out 
^C^\*** Error in `./a.out': double free or corruption (top): 0x0867f008 ***
已放弃 (核心已转储)
*/
void sig_handler(int s){
	printf("signal is %d, fclose wrok\n", s);
	fclose(fp);
	sleep(10);
	exit(0);
}

int main(int argc, const char *argv[])
{
	fp = fopen(FNAM, "r");

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);

	while (1) {
		pause();
	}

	return 0;
}
