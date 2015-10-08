#include<stdio.h>

void func(const char * str){
	printf("%s", str);
}

void func1(char * str){
	printf("%s", str);
}

int main(int argc, char *argv[]){
	char *str = "abc";
	func(str);

	printf("=================\n");
	func1(str);

	return 0;
}

