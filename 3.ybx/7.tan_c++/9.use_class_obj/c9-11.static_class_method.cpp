#include <iostream>
using namespace std;

class Student
{
	public:
		Student(int, int, int);
		void total();
		static float average();
	private:
		int num;
		int age;
		float score;
		static float sum;
		static int count;
};

Student::Student(int m, int a, int s)
{
	num = m;
	age = a;
	score = s;
}

void Student::total()
{
	sum+=score;
	count++;
}

float Student::average()
{
	return (sum/count);
}

float Student::sum = 0;
int Student::count = 0;

int main(int argc, const char *argv[])
{
	Student stud[3]={
		Student(1,19,70),
		Student(2,19,80),
		Student(3,19,90)
	};
	int  i;
	for (i = 0; i < 3 ; i++) {
		stud[i].total();
	}
	cout<<"============================"<<endl;
	cout<<"average:"<<Student::average()<<endl;
	cout<<"============================"<<endl;
	cout<<"average:"<<stud[0].average()<<endl;
	cout<<"============================"<<endl;
	cout<<"average:"<<stud[1].average()<<endl;
	cout<<"============================"<<endl;
	cout<<"average:"<<stud[2].average()<<endl;

	return 0;
}
