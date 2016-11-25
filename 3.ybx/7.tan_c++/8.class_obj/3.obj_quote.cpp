#include <iostream>
using namespace std;

class Time{
	public:
		int hour;
};


int main(int argc, const char *argv[])
{
	Time t1, *p;
	p = &t1;
	p->hour = 123;
	cout << t1.hour;
	return 0;
}
