#include <stdio.h>
//5 4 3 2 1
int jiecheng(n){
	if (n==1) {
		return 1;
	}
	return n * jiecheng(n-1) ;
}

int main(int argc, const char *argv[])
{
	int num = jiecheng(5);
	printf("%d\n", num);
	return 0;
}
