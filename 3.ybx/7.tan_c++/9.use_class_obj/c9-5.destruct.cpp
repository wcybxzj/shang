#include <iostream>
#include <string>

using namespace std;

class Student{
	public:
		Student(int n, string nam, char s)
		{
			num=n;
			name=nam;
			sex=s;
			cout<<"num:"<<num;
			cout<<" Construct called."<<endl;
		}

		~Student()
		{
			cout<<"num:"<<num;
			cout << " Destructor called."<<endl;
		}

		void display()
		{
			cout << "num:" << num << endl;
			cout << "name:" << name << endl;
			cout << "sex:" << sex << endl;
		}
	private:
		int num;
		string name;
		char sex;
};

int main(int argc, const char *argv[])
{
	Student s1(11, "ybx", 'm'),\
		s2(22, "ly", 'f');
	s1.display();
	s2.display();
	return 0;
}
