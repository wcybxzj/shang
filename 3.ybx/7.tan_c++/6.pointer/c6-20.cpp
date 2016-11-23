#include <iostream>
using namespace std;

void swap(int &a, int &b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}

int main(int argc, const char *argv[])
{
	int a = 123;
	int b = 456;
	swap(a, b);
	cout << a << b << endl;
	return 0;
}
