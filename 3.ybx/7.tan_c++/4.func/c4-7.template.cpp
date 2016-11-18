#include <iostream>
using namespace std;
template <typename T>

T max(T a, T b, T c){
	if (b>a) {
		a = b;
	}
	if (c>a) {
		a = c;
	}
	return a;
}


int main(int argc, const char *argv[])
{
	int a =1, b=3, c=3, re;
	long a1 = 11, b1=22, c1=33, re1;
	re = max(a, b, c);
	re1 = max(a1, b1, c1);
	cout << "re=" << re <<endl;
	cout << "re1=" << re1 <<endl;
	return 0;
}
