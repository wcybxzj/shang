#include <iostream>
using namespace std;

//测试1
void func1(int a =123)
{
	cout << a<< endl;
}

//测试2
void func2(int a = 456);

int main(int argc, const char *argv[])
{
	func1();
	func2();
	return 0;
}

void func2(int a)
{
	cout << a <<endl;
}
