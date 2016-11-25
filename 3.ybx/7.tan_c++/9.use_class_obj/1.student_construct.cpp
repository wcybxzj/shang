#include <iostream>
#include <string.h>
using namespace std;

class Student{
	public:
		Student(int n, char s, const char *nam): \
			num(n), sex(s) {strcpy(name, nam);}
		void display();
	private:
		int num;
		char sex;
		char name[20];
};

void Student::display(){
	cout << num << endl;
	cout << sex << endl;
	cout << name << endl;
}

int main(int argc, const char *argv[])

	Student s1(111, 'm', "yang");
	s1.display();
	return 0;
}
