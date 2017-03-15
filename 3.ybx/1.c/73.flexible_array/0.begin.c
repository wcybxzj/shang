#include <stdio.h>

//资料来源:http://coolshell.cn/articles/11377.html
//
//知识铺垫1:
//段错误的产生是对非法内存的访问:
//int *p=非法内存地址;不会报错.也就是个野指针
//但是如果访问野指针的内容则会报错

//知识点铺垫2:
void test1()
{
	char s[0];
	//对于数组 char s[10]来说，
	//数组名 s 和 &s 都是一样的都是数组首元素的地址
	printf("%x\n",s);//c50802b0
	printf("%x\n",&s);//c50802b0

	//可以看到char[0]不占空间
	printf("%d\n", sizeof(char [0]));//0
	printf("%d\n", sizeof(char *));//8
}

//知识点铺垫3:
void test2()
{
	//4+2+2+8+10=26
	//结构体最大类型是8结果32
	struct test{
		int i;//4
		short c;//2+2
		char *p;//8
		char s[10];//10
	};//32
	printf("%d\n", sizeof(char*));//8
	printf("%d\n", sizeof(struct test));//32

	struct test *pt=NULL;
	printf("&i = %x\n", &pt->i); //因为操作符优先级，我没有写成&(pt->i)
	printf("&c = %x\n", &pt->c);
	printf("&p = %x\n", &pt->p);
	printf("&s = %x\n", pt->s); //等价于 printf("%x\n", &(pt->s) );//test1()有测试
}

//问题:
//test3和test4两个段错误的位置为什么不同?
//char *s 和 char s[0]有什么差别呢？

//分析1:
//test3和test4的相同点:
//f.a被初始化成空指针,相当于f.a=NULL,相当于现在把f.a布置在内存的0空间上

//分析2:
//数组名和指针变量的区别:
//test3和test4的不同点:
//test3():
//char s[0];数组名s是数组首元素的的地址也是数组的地址
//if(f.a->s),这里s只是一个内存地址值4,
//这里不报错是因为虽然这个内存地址非法还没真的去访问这个内存地址
//printf("%s", f.a->s);//段错误,printf要访问非法内存中的值
//printf("%x", f.a->s);//正常输出地址4,printf打印s的地址
//注意printf的不同格式带来的不同的访问方式

//test4():
//char *s是指针
//if(f.a->s)这里就会去访问s内存地址的内容

//查看段错误发生的位置
//ulimit -c 10000
//./0.begin
//gdb 0.begin core.15776
void test3()
{
	struct str{
		int len;
		char s[0];
	};

	struct foo {
		struct str *a;
	};

	printf("%d\n", sizeof(struct str));//4
	printf("%d\n", sizeof(struct foo));//8

	struct foo f={0};//相当于f.a=NULL; f.a被理论上布置在内存地址0(非法地址),实际没有布置
	if (f.a->s) {
		//printf("%s\n", f.a->s);//段错误
		printf("%x\n",f.a->s);//正常输出数组地址4
	}
}

//查看段错误发生的位置
//ulimit -c 10000
//./0.begin
//gdb 0.begin core.15776
void test4()
{
	struct str{
		int len;
		char *s;
	};

	struct foo {
		struct str *a;
	};

	printf("%d\n", sizeof(struct str));//16
	printf("%d\n", sizeof(struct foo));//8

	struct foo f={0};//相当于f.a=NULL; f.a被理论上布置在内存地址0(非法地址),实际没有布置
	printf("%x\n", &(f.a->s));//8

	if (f.a->s) {//段错误
		printf("111111\n");//不能注释掉才能出现上面的段错误
	}
	return;
}

//修改test4()让 char*s的段错误位置和test1()一样
//ulimit -c 10000
//./0.begin
//gdb 0.begin core.15776
void test5()
{
	struct str{
		int len;
		char *s;
	};

	struct foo {
		struct str *a;
	};

	printf("%d\n", sizeof(struct str));//16
	printf("%d\n", sizeof(struct foo));//8

	struct foo f={0};//相当于f.a=NULL; f.a被理论上布置在内存地址0(非法地址),实际没有布置
	if (&(f.a->s)) {
		printf("%x\n",&(f.a->s));//8
	}
}


int main(int argc, char** argv) {
	//test1();
	//test2();
	test3();
	//test4();
	//test5();
	return 0;
}
