#include <iostream>
using namespace std;

class Time{
	public:
		Time(){minute=0, second=0;}
		Time(int m, int s){
			minute = m;
			second = s;
		}
		Time operator++();//前置++
		Time operator++(int);//后置++
		void display(){cout<<minute<<":"<<second<<endl;}
	private:
		int minute;
		int second;
};

Time Time::operator++()
{
	if (++second>=60) {
		second-=60;
		minute++;
	}
	return *this;
}

Time Time::operator++(int)
{
	Time temp(*this);
	second++;
	if (second>=60) {
		second-=60;
		minute++;
	}
	return temp;
}
int main(int argc, const char *argv[])
{
	Time time1(34, 59), time2;
	time1.display();//34:59
	++time1;
	time1.display();//35:00
	time2 = time1++;
	time1.display();//35:01
	time2.display();//35:00
	return 0;
}
