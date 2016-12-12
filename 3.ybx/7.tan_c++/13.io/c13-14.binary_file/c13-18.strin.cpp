#include <iostream>
#include <strstream>
using namespace std;

int main(int argc, const char *argv[])
{
	//c->a
	int i;
	char c[50] = "12 34 56 78 90 91 92 93 94 95";
	int arr[10];
	cout<<c<<endl;
	istrstream strin(c, sizeof(c));
	for (i = 0; i < 10; i++) {
		strin >> arr[i];
	}
	for (i = 0; i < 10; i++) {
		cout << arr[i] << " ";
	}
	cout << endl;
	return 0;
}
