#include <iostream>
#include <iomanip>
using namespace std;


int main(int argc, const char *argv[])
{
	int a = 10;
	int &b = a;
	a = a*a;
	cout << a << setw(6) <<b << endl;
	b = b/5;
	cout << a << setw(6) <<b << endl;
	return 0;
}
