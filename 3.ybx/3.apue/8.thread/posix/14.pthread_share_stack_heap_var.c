#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static void* func1(void *p){
	printf("a:%d\n",*((int*)p));//123
	*((int*)p)=456;
}

static void* func2(void *p){
	printf("a:%d\n",*((int*)p));//456
	*((int*)p)=789;
}

//测试1:测试栈变量在线程之间的共享
//结果:
//123
//456
//789
void test_stack_var()
{
	int a =123;
	int i;
	pthread_t tid;
	pthread_create(&tid, NULL, func1, &a);
	sleep(1);
	pthread_create(&tid, NULL, func2, &a);
	sleep(1);
	printf("a:%d\n",a);//789
	pause();

}

static void* func3(void *p){
	printf("%d\n", *(*((int **)p)));//123
	*(*((int **)p))=456;
}

static void* func4(void *p){
	printf("%d\n", *(*((int **)p)));//456
	*(*((int **)p))=789;
}

//测试2:测试堆变量在线程之间的共享
//结果:
//123
//456
//789
void test_heap_var()
{
	int *p = malloc(sizeof(int));
	*p = 123;
	int i;
	pthread_t tid;
	pthread_create(&tid, NULL, func3, &p);
	sleep(1);
	pthread_create(&tid, NULL, func4, &p);
	sleep(1);
	printf("%d\n",*p);//789
	pause();

}

//线程之间 栈变量 堆变量 是否可以共享的问题
//全局变量在线程之间是共享的
int main(int argc, const char *argv[])
{
	//test_stack_var();
	test_heap_var();
	return 0;
}
