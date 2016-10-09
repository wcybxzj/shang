#include <stdio.h>
#include <stdlib.h>

typedef int MYFILE;

//猜测FILE *fopen();  
//FILE* 指向的是 栈? 静态区? 堆?

//直接报错
//指针指向栈地址不能返回
//MYFILE *stack_fopen(){
//	MYFILE a = 100;
//	return &a;
//}

//指针指向局部静态区问题是
//如果函数打开2次就会改变指针指向的内容
MYFILE *static_fopen(){
	static MYFILE a; 
	a = rand()%1000;
	return &a;
}

MYFILE *heap_fopen(){
	MYFILE* a; 
	a = malloc(sizeof(*a));
	*a = 123;
	return a;
}

int head_close(MYFILE *fp){
	free(fp);
	return 1;
}

int main(int argc, const char *argv[])
{
	MYFILE *fp1, *fp2, *fp3;

	//栈:
	//fp1 = stack_fopen();
	//printf("%d\n", *fp1);

	//静态区:
	//指针指向静态区的
	//问题第二次调用,会改变之前指针的指向的内容
	fp2 = static_fopen();
	printf("%d\n", *fp2);//383
	fp3 = static_fopen();
	printf("%d\n", *fp3);//886
	printf("%d\n", *fp2);//886

	//堆:
	fp2 = heap_fopen();
	printf("%d\n", *fp2);
	head_close(fp2);
	return 0;
}
