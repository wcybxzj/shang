#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static void* func1(void *p){
	printf("fcun1() a:%d\n",*((int*)p));//123
	*((int*)p)=456;
	printf("func1() a:%d\n",*((int*)p));//456
	sleep(3);
	printf("func1() a:%d\n",*((int*)p));//456
}

static void* func2(void *p){
	sleep(1);
	printf("func2() a:%d\n",*((int*)p));//456
	*((int*)p)=789;
}

/*
//测试1:测试栈变量在线程之间的共享
//结果:
./14.pthread_share_stack_heap_var 
fcun1() a:123
func1() a:456
func2() a:456
func1() a:789
test_stack_var() a:789
*/

void test_stack_var()
{
	int a =123;
	int i;
	pthread_t tid;
	pthread_create(&tid, NULL, func1, &a);
	pthread_create(&tid, NULL, func2, &a);
	sleep(5);
	printf("test_stack_var() a:%d\n",a);
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
	test_stack_var();
	//test_heap_var();
	return 0;
}
