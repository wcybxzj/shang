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

class Student1: private Student
{
	public:
		void get_value_1()
		{
			get_value();
			cout<<"age,addr"<<endl;
			cin>>age>>addr;
		}

		void display_1()
		{
			display();
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
	//stud1.get_value();//error
	stud1.get_value_1();
	//stud1.display();//error
	stud1.display_1();
	return 0;
}
