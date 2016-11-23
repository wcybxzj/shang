#include <iostream>
#include <string>
using namespace std;

struct Student {
	int num;
	string name;
	float score[3];
} stu = {123,"ybx", 99.9, 88.9, 77.1};

void print(Student & stud)
{
	stud.num = 456;
	cout << stud.num << endl;
	cout << stud.name << endl;
	cout << stud.score[0] <<endl;
	cout << stud.score[1] << endl;
	cout << stud.score[2] << endl;
}

int main(int argc, const char *argv[])
{
	print(stu);
	return 0;
}

