#include <iostream>
#include <iomanip>
using namespace std;
int main(int argc, const char *argv[])
{
	int a = 21;
	//cout<<setbase(8)<<endl;
	//强制输出数字的进制,8位0, 16位0x
	cout.setf(ios::showbase);
	cout<<a<<endl;

	cout.unsetf(ios::dec);//必须
	cout.setf(ios::hex);
	cout<<a<<endl;

	cout.unsetf(ios::hex);//必须
	cout.setf(ios::oct);
	cout<<a<<endl;

	const char *p = "China";
	cout.width(10);
	cout<<p<<endl;

	cout.width(10);
	cout.fill('*');
	cout<<p<<endl;

	double pi = 22.0/7.0;
	cout.setf(ios::scientific);
	cout.width(14);
	cout<<pi<<endl;

	cout.unsetf(ios::scientific);
	cout.setf(ios::fixed);
	cout.width(12);
	cout.setf(ios::showpos);
	cout.setf(ios::internal);
	cout.precision(6);
	cout<<pi<<endl;

	return 0;
}

