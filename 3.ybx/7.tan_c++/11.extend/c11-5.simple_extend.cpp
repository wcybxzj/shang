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

	protected:
		int num;
		string name;
		char sex;

};


class Student1:public Student
{
	public:
		Student1(int n, string nam, char s, int a, string ad): \
			Student(n, nam, s), age(a), addr(ad){}
		void show()
		{
			cout<<"num:"<<num<<endl;
			cout<<"name:"<<name<<endl;
			cout<<"sex:"<<sex<<endl;
			cout<<"age:"<<age<<endl;
			cout<<"addr:"<<addr<<endl;
		}
		~Student1(){}
	private:
		int age;
		string addr;
};

int main(int argc, const char *argv[])
{
	Student1 s1(1, "ybx", 'm', 19, "bj");
	s1.show();
	return 0;
}
