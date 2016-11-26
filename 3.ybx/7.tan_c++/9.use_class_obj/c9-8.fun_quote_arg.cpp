#include <iostream>
using namespace std;

class Time{
	public:
		Time(int h, int m, int s): \
			hour(h),minute(m),sec(s){}
		int hour;
		int minute;
		int sec;
};

void fun(Time &t)
{
	t.hour = 123;
}

int main(int argc, const char *argv[])
{
	Time t1(10, 20, 30);
	fun(t1);
	cout << t1.hour <<endl;
	return 0;
}
