#include <iostream>
#include <string>
using namespace std;

class Student{
	public:
		Student(int n, string nam, char s)
		{
			num = n;
			name = nam;
			sex = s;
		}
		~Student(){}

		void display()
		{
			cout<<"num:"<<num<<endl;
			cout<<"name:"<<name<<endl;
			cout<<"sex:"<<sex<<endl;
		}
	protected:
		int num;
		string name;
		char sex;
		
};

class Student1:public Student
{
	public:
		Student1(int n, string nam, char s, 
				int n1, string nam1, char s1, int a, string ad): \
			Student(n, nam, s), monitor(n1, nam1, s1), age(a), addr(ad){}

		void show()
		{
			//cout<<"num:"<<num<<endl;
			//cout<<"name:"<<name<<endl;
			//cout<<"sex:"<<sex<<endl;
			display();
			cout<<"age:"<<age<<endl;
			cout<<"addr:"<<addr<<endl;
		}
		~Student1(){}
		void show_mointor()
		{
			cout<<endl;
			cout<<"class mointor is:"<<endl;
			monitor.display();
		}
	private:
		Student monitor;
		int age;
		string addr;
};

int main(int argc, const char *argv[])
{
	Student1 s1(1, "ybx", 'm', 2, "dabing",'m', 19, "bj");
	s1.show();
	s1.show_mointor();
	return 0;
}
