#include <iostream>
#include <string>
using namespace std;

//多重派生
class Student{
	public:
		Student(int n, string nam)
		{
			num = n;
			name = nam;
		}
		void display(){
			cout<<num<<endl;
			cout<<name<<endl;
		}
	protected:
		int num;
		string name;
};


class Student1:public Student{
	public:
		Student1(int n, char nam[10], int a):Student(n, nam)
		{
			age = a;
		}

		void show()
		{
			display();
			cout<<age<<endl;
		}
	private:
		int age;
};


class Student2:public Student1
{
	public:
		Student2(int n, char nam[10], int a, int s):Student1(n, nam, a)
		{
			score = s;
		}
		void show_all()
		{
			show();
			cout<<score<<endl;
		}
	private:
		int score;
};

int main(int argc, const char *argv[])
{
	Student2 stud(10010, "ybx", 17, 89 );
	stud.show_all();
	return 0;
}
