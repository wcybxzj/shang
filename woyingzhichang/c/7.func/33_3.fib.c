#include <stdio.h>


int fib(int n){
	if (n==1 || n ==2) {
		return 1;
	}
	return fib(n-1)+ fib(n-2);

}

//1, 1 ,2, 3,5, 8
int main(int argc, const char *argv[])
{
	printf("%d\n", fib(6));//8
	return 0;
}
