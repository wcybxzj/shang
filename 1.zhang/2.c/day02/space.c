#include <stdio.h>

int main(void)
{
	char ch;

	while (1) {
		ch = getchar();	
		if (ch == EOF) {
			break;
		}
		if (ch == ' ') {
			putchar(ch);
			while ((ch = getchar()) == ' ');
			putchar(ch);
		} else {
			putchar(ch);
		}
	}

#if 0
	int flag = 0;
	while (1) {
		ch = getchar();
		if (ch == EOF) {
			break;
		}
		if (ch == ' ') {
			if (++flag == 1) {
				putchar(ch);
			}
		} else {
			putchar(ch);
			flag = 0;	
		}
	}
#endif	

	return 0;
}
