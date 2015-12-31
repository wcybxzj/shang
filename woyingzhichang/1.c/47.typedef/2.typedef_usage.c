#include <stdio.h>
#include <stdlib.h>

void func(int a)
{
	printf("ok\n");
	return;
}

int *func2(int a){
	int *p = malloc(sizeof(int));
	*p = a;
	return p;
}

int main(int argc, const char *argv[])
{
	//例子1:
	typedef int ARR[6];
	ARR var1;//int var1[6];

	//例子2:
	//结构体一般放在全局最上面放这里是为演示方便
	struct  node_st{
		int i;
		float f;
	};
	typedef struct node_st NODE;
	NODE var2;//struct node_st var2;

	typedef struct node_st *NODEP;
	NODEP var3;//struct node_st * var3;
	NODE *var4;//同上

	//例子3:效果和例2一样
	typedef struct {
		int i;
		float f;
	}NODE1, *NODEP1;

	//例子4:
	//参数int 返回值int的匿名函数类型 取名为FUNC
	typedef int FUNC(int);//int(int) FUNC
	FUNC f;//int f(int);

	//例子5:
	//返回值位int *的函数类型　取名位FUNC2
	typedef int *FUNC2(int);//int *(int) FUNC2
	FUNC2 p;//int *p(int);

	//例子６:
	typedef int* (*FUNCP) (int);
	FUNCP funcp = func2;//int *(*funcp)(int)
	int *tmp = (*func2)(123);
	printf("%d\n", *tmp);

	return 0;
}


