#include <stdio.h>

int step(int n)
{
	if (n==0) {
		return 0;
	}else if(n==1){
		return 1;
	}else if(n==2){
		return 2;
	}else if(n==3){
		return 4;
	}
	return step(n-1) + step(n-2) + step(n-3);
	
}

int main(int argc, const char *argv[])
{
	int num = step(9);
	printf("%d\n", num);
	return 0;
}
