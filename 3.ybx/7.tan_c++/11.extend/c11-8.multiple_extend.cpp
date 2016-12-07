#include <iostream>
#include <string>
using namespace std;

class Teacher{
	protected:
		string name;
		int age;
		string title;

	public:
		Teacher(string nam, int a, string t)
		{
			name = nam;
			age = a;
			title = t;
		}

		void display()
		{
			cout<<name<<endl;
			cout<<age<<endl;
			cout<<title<<endl;
		}
};


class Student{
	protected:
		string name;
		char sex;
		float score;
	public:
		Student(string nam, char s, float sco)
		{
			name = nam;
			sex = s;
			score = sco;
		}
		void display()
		{
			cout<<name<<endl;
			cout<<sex<<endl;
			cout<<score<<endl;
		}
};


class Graduate: public Teacher, public Student{
	private:
		int wage;//工资

	public:
		Graduate(string nam, int a, string t, char s, float sco, int w):
			Teacher(nam, a, t), Student(nam, s, score), wage(w){}

		void show()
		{
			cout<<Teacher::name<<endl;
			cout<<Student::name<<endl;
			cout<<age<<endl;
			cout<<title<<endl;
			cout<<sex<<endl;
			cout<<score<<endl;
			cout<<wage<<endl;
		}
};


int main(int argc, const char *argv[])
{
	Graduate g1("ybx", 30, "God", 'm', 100, 100000);
	g1.show();
	return 0;
}
