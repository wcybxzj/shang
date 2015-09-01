#include <stdio.h>

int main(void)
{
	int lc = 1;
	int wc = 0;
	int cc = 0;	
	int ch;
	int flag = 1;

	//abc ef
	while ((ch = getchar()) != EOF) {
		cc++;
		if (ch == '\n') {
			lc ++;
		}
	   	if (ch == ' ' || ch == '\t' || ch == '\n') {
			flag = 1;
		} else if (flag == 1){
			wc ++;
			flag = 0;
		}
	}
	printf("lc = %d, wc = %d, cc = %d\n", lc, wc, cc);

	return 0;
}
