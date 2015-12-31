#include <stdio.h>

int main(void)
{
	int cnt[11] = {};
	int ch;
	int i;

	while (1) {
		ch = getchar();	
		if (ch == EOF) {
			break;
		}
		if (ch >= '0' && ch <= '9') {
			cnt[ch-'0'] ++;
		} else {
			cnt[10]++;	
		}
	}

	for (i = 0; i < 10; i++ ) {
		printf("字符%d的个数是%d\n", i, cnt[i]);
	}
	printf("其他字符的个数是%d\n", cnt[10]);

	return 0;
}
