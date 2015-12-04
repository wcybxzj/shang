#include <stdio.h>

int add(int a, int b){
	return a + b;
}

int sub(int a, int b){
	return a - b;
}


int main(int argc, const char *argv[])
{
	int i;
	int ret;
	int a = 3, b = 5;
	int (*func_arr[2])(int, int);
	func_arr[0] = add;
	func_arr[1] = sub;
	for (i = 0; i < sizeof(func_arr)/sizeof(*func_arr); i++) {
		printf("%d\n", func_arr[i](a, b));
	}


	return 0;
}
