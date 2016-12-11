#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, const char *argv[])
{
	int a=15;
	cout<<dec<<a<<endl;
	cout<<hex<<a<<endl;
	cout<<setbase(8)<<a<<endl;

	const char *pt="China";
	cout<<setw(10)<<pt<<endl;
	cout<<setfill('*')<<setw(10)<<pt<<endl;

	double pi=10.0/3.0;
	cout<<pi<<endl;
	cout<<setiosflags(ios::scientific)<<setprecision(8);
	cout<<pi<<endl;
	cout<<setprecision(4);
	cout<<pi<<endl;
	cout<<setiosflags(ios::fixed)<<pi<<endl;
	return 0;
}
