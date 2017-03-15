#include <stdio.h>

//使用gdb查看内存分布
/*
gdb 1.basic
(gdb) b 21
(gdb) run
(gdb) p t
$1 = {i = 2065759136, p = 0x4004b0 "H\211l$\330L\211d$\340H\215-\313\001 "}
(gdb) p &t
$2 = (struct test *) 0x7fffffffe290
(gdb) p &(t.i)
$3 = (int *) 0x7fffffffe290
(gdb) p &(t.p)
$4 = (char **) 0x7fffffffe298 (偏移量是8)
*/
void func1()
{
	struct test{
	    int i;
	    char *p;//区别
	};
	struct test t;//gdb在这设置断点
	printf("%d\n", sizeof(struct test));//16
}

//我们可以看到，t.i的地址和t的地址是一样的，t.p的址址相对于t的地址多了个4。
//说白了，t.i 其实就是(&t + 0x0), t.p 的其实就是 (&t + 0x4)。
//0x0和0x4这个偏移地址就是成员i和p在编译时就被编译器给hard code了的地址。
//于是，你就知道，不管结构体的实例是什么——访问其成员其实就是加成员的偏移量。
/*
(gdb) b 44
(gdb) run
(gdb) p t
$1 = {i = 2065759136, p = 0x7fffffffe290 "\240\373 {<"}
p的地址是:0x7fffffffe290,值是:随机值,p是野指针
(gdb) p &t
$2 = (struct test *) 0x7fffffffe290
(gdb) p &(t.i)
$3 = (int *) 0x7fffffffe290
(gdb) p &(t.p)
$4 = (char (*)[]) 0x7fffffffe294 (偏移量是4)
*/
void func2()
{
	struct test{
	    int i;
	    char p[0];//区别
	};
	struct test t;//gdb在这设置断点
	printf("%d\n", sizeof(struct test));//4
}

//比较结构体中char p[0]和char *p的区别
//char p[0]:在结构体中不占空间
//char *p  :在结构体中8字节
int main(int argc, const char *argv[])
{
	func1();
	func2();
	return 0;
}
