#include <iostream>
using namespace std;

int main(int argc, const char *argv[])
{
	int a, b;
	extern int max(int , int);
	cin >> a >>b;
	cout << max(a,b) <<endl;

	return 0;
}
