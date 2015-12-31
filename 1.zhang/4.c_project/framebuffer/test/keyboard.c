#include <stdio.h>
#include <termios.h>
#include <unistd.h>


int main(void)
{
	struct termios tc;
	int ch;

	tcgetattr(0, &tc);

	tc.c_lflag &= (~ICANON);		
	tc.c_lflag &= (~ECHO);

	tcsetattr(0, TCSANOW, &tc);

	while (1) {
		ch = getchar();

		if (ch == 'a') {
			printf("this is a test\n");
		}
	}

	return 0;
}
