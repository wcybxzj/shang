#include <iostream>
#include <cstring>
using namespace std;

//1.凡希望数据成员不被修改,可以声明为常对象来达到目的
//2.常对象只能调用对象中的常成员函数
//3.必须使用参数初始化表对常参数进行初始化
//4.一定要修改常对象中某数据将其设置为mutable
class Time{
	public:
		void get_time() const;
		void change() const;
		void say();
		Time(int h, int m, int s, int n):\
			hour(h) ,minute(m), second(s), num(n) { }
	private:
		const int hour;
		const int minute;
		mutable int second;
		int num;
};

void Time::get_time() const{
	//hour = 222;//error hour is read-only
	//num =456;//error num is read-only
	cout<<"hour:"<<hour<<endl;
	cout<<"minute:"<<minute<<endl;
	cout<<"second:"<<second<<endl;
	cout<<"num:"<<num<<endl;
	second = 12345;
	cout<<"second:"<<second<<endl;
}

void Time::say(){
	cout << "hello" << endl;
	second = 4444;
	cout<<"second:"<<second<<endl;
}

//常对象只能访问常函数
void test1()
{
	const Time t1(10, 20, 30, 123);
	t1.get_time();
	//t1.say();//error
}

//普通对象可以访问常函数和普通函数
void test2()
{
	Time t2(1, 2, 3, 4);
	t2.get_time();
	t2.say();//ok
}

int main(int argc, const char *argv[])
{
	//test1();
	test2();
	return 0;
}
