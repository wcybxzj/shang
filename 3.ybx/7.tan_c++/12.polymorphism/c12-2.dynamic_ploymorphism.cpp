#include <iostream>
#include <string>
using namespace std;

//动态多态性-虚函数
class Student
{
	public:
		Student(int, string, float);
		virtual void display();
	protected:
		int num;
		string name;
		float score;
};

Student::Student(int n, string nam, float s)
{
	num = n;
	name = nam;
	score = s;
}

void Student::display()
{
	cout<<num<<endl;
	cout<<name<<endl;
	cout<<score<<endl;
}

class Graduate:public Student
{
	private:
		float pay;
	public:
		Graduate(int, string, float, float);
		void display();
};

Graduate::Graduate(int n, string nam, float s, float p):Student(n, nam, s), pay(p){}

void Graduate::display()
{
	cout<<num<<endl;
	cout<<name<<endl;
	cout<<score<<endl;
	cout<<pay<<endl;
}

int main(int argc, const char *argv[])
{
	Student s1(123, "ybx", 110);
	Graduate g1(456, "wc", 99, 2.22);
	Student *pt = &s1;
	pt->display();
	pt = &g1;
	pt->display();
	return 0;
}
