#include <stdio.h>
#include <signal.h>

void func1()
{
	static num = 1;
	int i;
	int j;
	for (i = 0 ; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (num%2) {
				printf("\33[44m  ");
			}else{
				printf("\33[43m  ");
			}
		}
		printf("\n");
	}
	num++;
	printf("\33[0m");
	system("clear");
	alarm(1);
}


int main(void)
{
	signal(SIGALRM, func1);
	alarm(1);
	while (1);
	return 0;
}
