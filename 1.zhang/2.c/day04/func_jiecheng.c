#include <stdio.h>
int jiecheng(int num, int *p);

int main(int argc, const char *argv[])
{
	int p;
	if (jiecheng(4, &p)) {
		printf("ok\n");
		printf("%d\n", p);
	}else {
		printf("fail\n");
	}

	printf("\n===========\n");
	if (jiecheng(-10, &p)) {
		printf("ok\n");
		printf("%d\n", p);
	}else {
		printf("fail\n");
	}


	return 0;
}

//4
//1 *2 *3 *4
int jiecheng(int num, int *p){
	int ret =1;
	int i;
	if(num < 0) {
		return 0;
	}else{
		for (i = 1; i <= num; i++) {
			ret *= i;
		}
		*p = ret;
		return 1;
	}
}
