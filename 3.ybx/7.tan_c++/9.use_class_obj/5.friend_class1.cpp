#include <iostream>
using namespace std;

//情况1:class Time 对普通函数display友元
//dislpay函数可以访问class Time对象的private属性
class Time{
	public:
		Time(int , int, int);
		friend void display(Time &);

	private:
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

void display(Time &t)
{
	cout<<t.hour<<":"<<t.minute<<t.sec<<endl;
}

int main(int argc, const char *argv[])
{
	Time t1(11, 22, 33);
	display(t1);
	return 0;
}
