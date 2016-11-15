#include <iostream>
using namespace std;

//make 0.++
//g++     0.++.cpp   -o 0.++
//0.++.cpp: In function ‘int main(int, const char**)’:
//0.++.cpp:7: error: lvalue required as increment operand
//make: *** [0.++] Error 1
//意思是自增需要一个lvalue，可是(-a)是一个表达式
int main(int argc, const char *argv[])
{
	int a=1;
	(-a)++;
	cout << a;
	return 0;
}
