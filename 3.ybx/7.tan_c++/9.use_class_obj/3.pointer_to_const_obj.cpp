#include <iostream>
using namespace std;

class Time{
	public:
		Time(int h):hour(h){}
		int hour;
};

int main(int argc, const char *argv[])
{
	Time t1(100);
	const Time *p = &t1;
	t1.hour=200;
	//(*p).hour=300;// Time::hour is read-only
	cout << (*p).hour<<endl;
	return 0;
}

