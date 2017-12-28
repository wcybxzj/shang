#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//gcc 1.c -lm
int main(){
	//sqrt(-1) is not a number
	if (isnan(sqrt(-1))) {
		printf("sqrt(-1) 的结果不是一个数字\n");
	}
	return 0;
}
