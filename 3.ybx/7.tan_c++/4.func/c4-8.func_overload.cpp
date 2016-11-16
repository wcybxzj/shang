#include <iostream>
using namespace std;

void test1();
void test2();
//根据参数不同来进行重载
int max(int a, int b, int c=123);
double max(double a, double b, double c=123.456);
long max(long a, long b, long c=123456789);

//根据参数个数进行重载
void func1()
{
	cout << "this is 111111\n";
}

void func1(int a)
{
	cout <<"this is 22222\n";
}

int main(int argc, const char *argv[])
{
	//test1();
	test2();
	return 0;
}

void test1()
{
	max(111, 222);
	max(1.11, 2.22);
	max(3333333L, 444444L);

	cout << "==================" << endl;
	cout << "==================" << endl;
	cout << "==================" << endl;

	long a, b, c;
	cout << "plz enter 3 num" << endl;
	cin >> a >> b >> c;
	max(a, b, c);
}

void test2()
{
	func1();
	func1(222);
}


int max(int a, int b, int c){
	cout <<"int" << endl;
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "c:" << c << endl;
}

double max(double a, double b, double c)
{
	cout << "double" << endl;
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "c:" << c << endl;
}

long max(long a, long b, long c)
{
	cout << "long" << endl;
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "c:" << c << endl;
}
