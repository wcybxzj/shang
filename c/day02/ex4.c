#include <stdio.h>
int main(int argc, const char *argv[])
{
	int c;
	int in = 0;//out
	while ((c=getchar()) != EOF) {
		if (in && c == ' ') {
			in = 0;
		}else if(c== ' '){
			continue;
		}else{
			in =1;
		}
		printf("%c", c);
	}
	return 0;
}
