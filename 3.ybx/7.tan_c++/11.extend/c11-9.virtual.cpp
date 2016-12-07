#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class Person{
	protected:
		char name[20];
		char sex;
		int age;

	public:
		Person(const char *nam, char s, int a)
		{
			strcpy(name, nam);
			sex = s;
			age = a;
		}
};

class Teacher:virtual public Person
{
	protected:
		char title[20];

	public:
		Teacher(const char *nam, char s, int a, const char *t):Person(nam, s, a){
			strcpy(title, t);
		}
};

class Student:virtual public Person
{
	protected:
		float score;
	public:
		Student (const char *nam, char s, int a, float sco): 
			Person(nam, s, a), score(sco){}
};



class Graduate:public Teacher, public Student
{
	private:
		float wage;

	public:
		Graduate(const char *nam, char s, int a, const char *t, float sco, float w): 
			Person(nam, s, a), Teacher(nam, s, a, t), Student(nam, s ,a, sco), wage(w){}
		void show(){
			cout<<name<<endl;
			cout<<sex<<endl;
			cout<<age<<endl;
			cout<<title<<endl;
			cout<<score<<endl;
			cout<<wage<<endl;
		}
};

int main(int argc, const char *argv[])
{
	Graduate g1("ybx", 'm', 30, "God", 100, 200);
	g1.show();
	return 0;
}
