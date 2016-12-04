#include <iostream>
#include <string>
using namespace std;

class Student{
	protected:
		int num;
		string name;
		char sex;
};

class Student1: protected Student
{
	public:
		void get_value()
		{
			cout<<"num, name, sex"<<endl;
			cin>>num>>name>>sex;
			cout<<"age,addr"<<endl;
			cin>>age>>addr;
		}

		void display()
		{
			cout<<"num: "<<num<<endl;
			cout<<"name: "<<name<<endl;
			cout<<"sex: "<<sex<<endl;
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
	stud1.display();
	return 0;
}
