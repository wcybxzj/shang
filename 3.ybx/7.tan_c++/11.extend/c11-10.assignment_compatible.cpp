#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class Student{
	public:
		Student(int n, string nam, float sco);
		void display();
	private:
		int num;
		string name;
		float score;
};

Student::Student(int n, string nam, float sco)
{
	num = n;
	name = nam;
	score = sco;
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
		Graduate(int ,string, float, float);
		void display();
};

Graduate::Graduate(int n, string nam, float sco, float p):Student(n, nam, sco), pay(p){}

void Graduate::display()
{
	Student::display();
	cout<<pay<<endl;
}


int main(int argc, const char *argv[])
{
	Student s1(123, "ybx", 100);
	Graduate g1(456, "dabing", 200, 300);
	Student *pt= &s1;
	pt->display();
	pt = &g1;
	pt->display();
	return 0;
}
