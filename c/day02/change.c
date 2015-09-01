#include <stdio.h>

int main(void)
{
	char ch;

	printf("input a charator:\n");
	scanf("%c", &ch);

	if (ch >= 'a' && ch <= 'z') {
		ch = ch - ('a' - 'A');
	} else if (ch >= 'A' && ch <= 'Z') {
		ch = ch + ('a' - 'A');
	} else {
		printf("invalued...\n");	
		return -1;
	}
	printf("************%c***************\n", ch);

	return 0;
}
