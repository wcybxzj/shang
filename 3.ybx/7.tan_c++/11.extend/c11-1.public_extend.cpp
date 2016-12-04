#include <iostream>
#include <string>
using namespace std;

class Student{
	public:
		void get_value(){
			cout<<"num, name, sex"<<endl;
			cin>>num>>name>>sex;
		}

		void display()
		{
			cout<<"num: "<<num<<endl;
			cout<<"name: "<<name<<endl;
			cout<<"sex: "<<sex<<endl;
		}
	private:
		int num;
		string name;
		char sex;
};

class Student1: public Student
{
	public:
		void get_value_1()
		{
			cout<<"age,addr"<<endl;
			cin>>age>>addr;
		}

		void display_1()
		{
			//error 基类的private属性不能通过派生类的新方法访问
			//cout<<num<<endl;
			//cout<<name<<endl;
			//cout<<sex<<endl;
			cout<<"age:"<<age<<endl;
			cout<<"addr:"<<addr<<endl;
		}
	private:
		int age;
		string addr;
};

int main(int argc, const char *argv[])
{
	Student1 stud1;
	stud1.get_value();
	stud1.get_value_1();
	stud1.display();
	stud1.display_1();
	return 0;
}
