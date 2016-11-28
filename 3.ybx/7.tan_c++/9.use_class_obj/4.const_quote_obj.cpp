#include <iostream>
using namespace std;

class Time{
	public:
		Time(int, int, int);
		int hour;
		int minute;
		int sec;
};

Time::Time(int h, int m, int s)
{
	hour = h;
	minute = m;
	sec = s;
}

void fun1(Time &t)
{
	t.hour =123;
}

void fun2(const Time &t)
{
	t.hour =123;//error Time::hour read-only
}

int main(int argc, const char *argv[])
{
	Time t1(11, 22 ,33);
	fun1(t1);
	cout << t1.hour<< endl;
	//fun2(t1);
	cout << t1.hour<< endl;
	return 0;
}
