#include <iostream>
using namespace std;

class Student{
	public:
		void display();

	private:
		int num;
		string name;
		char sex;
};

void Student::display(){
	cout << "ok"<< endl;
	//cout << num << endl;
	//cout << name << endl;
	//cout << sex << endl;
}

int main(int argc, const char *argv[])
{
	Student s1;
	s1.display();
	return 0;
}
