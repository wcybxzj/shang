#include <iostream>
using namespace std;

//声明一个引用后不能再使之成为另外一个变量的引用
void test1()
{
	int a1, a2;
	int &b = a1;
	//int &b = a2;//error
}

//数组不能被引用
void test2()
{
	int a[5];
	//int &b[5]= &a;//error
}

//可以建立引用的引用
void test3()
{
	int a=3;
	int &b = a;
	int &c = b;
	//a=100;
	cout << c << endl;
}

//不能建立指向引用的指针
void test4()
{
	int a =3;
	int &b =a;
	//int *p = b;//error
	//cout << *p << endl;
}

//可以取引用的地址
void test5()
{
	int a=123;
	int &b =a;
	int *pt;
	pt = &b;
	cout << *pt;
}

void test6()
{
	int a=123;
	int &b = a;//b引用a
	cout << &b <<endl;//打印b的地址
}

int main(int argc, const char *argv[])
{
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	test6();
	return 0;
}
