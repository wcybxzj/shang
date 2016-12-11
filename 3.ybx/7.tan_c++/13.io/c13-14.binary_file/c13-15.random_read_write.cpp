#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;

struct Student
{
	char name[20];
	int num;
	int age;
	char sex;
};

void write_test()
{
}


void read_test()
{
	int i;
	Student stud[3];
	ifstream infile("stud.txt", ios::binary);
	if (!infile) {
		cerr<<"open err"<<endl;
		exit(1);
	}
	for (i = 0; i < 3; i++) {
		infile.read((char *) &stud[i], sizeof(stud[i]));
	}
	infile.close();
	for (i = 0; i < 3; i++) {
		cout<<stud[i].name<<endl;
		cout<<stud[i].num<<endl;
		cout<<stud[i].age<<endl;
		cout<<stud[i].sex<<endl;
	}
}


int main(int argc, const char *argv[])
{

	int i;
	Student stud[3] = {
		"dabing", 111, 100,'m',
		"wc", 222, 99, 'm',
		"ly",333, 98, 'f',
		"ybx",444, 94, 'm',
		"fbb",555, 93, 'f',
		"zx",666, 92, 'f'
	};
	fstream iofile("stud.txt", ios::in|ios::out|ios::binary);
	if (!iofile) {
		cerr<<"open error"<<endl;
		exit(1);
	}
	iofile.write((char *) stud, sizeof(stud));

	return 0;
}
