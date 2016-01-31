#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int i;
	for (i = 0; ; i++) {
		printf("%d\n");
		sleep(1);
	}

	//pid = fork();
	//if (pid == 0) {
	//	while (1) {
	//		sleep(1);
	//	}
	//}
	return 0;
}
