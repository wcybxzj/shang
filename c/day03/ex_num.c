#include <stdio.h>


int main(int argc, const char *argv[])
{
	int c, i;
	int int_c;
	int arr[11]={};
	while ((c=getchar()) !=EOF) {
		if (c >= '0' && c<='9') {
			int_c = c - '0';
			arr[int_c]++;
		}else{
			arr[10]++;
		}
	}

	for (i = 0; i < 11; i++) {
		printf("%d:%d\n",i ,arr[i]);
	}

	return 0;
}
