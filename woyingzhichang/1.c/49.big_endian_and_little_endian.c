#include <stdio.h>
#include <stdlib.h>

typedef union var_st{
	int var;
	char arr[4];
}VAR_ST;


//测试方法1:
void test1(){
	//int a = 0x12345678;
	int a = 120;
	char *p = (char*) &a;
	//printf("%c\n", p[0]);//x
	//printf("%d\n", p[0]);//120
	//printf("%x\n", p[0]);//78
	//printf("%c\n", *p);//x
	//printf("%d\n", *p);//120
	//printf("%x\n", *p);//78
	if (*p == 0x78) {
		printf("little-endian\n");
	}else{
		printf("big-endian\n");
	}
}

//测试方法2:
void test2(){
	VAR_ST v;
	v.var=0x12345678;
	//printf("%x", v.arr[0]);//78
	if (v.arr[0] == 0x78) {
		printf("little-endian\n");
	}else{
		printf("big-endian\n");
	}
}

int main(void){
	test1();
	test2();
	exit(0);
}
