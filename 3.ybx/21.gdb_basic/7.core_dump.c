#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>



void func1(){
	int *p = NULL;
	*p =1;
}



int main(int argc, char *argv[])
{
	func1();
	return 0;
}
