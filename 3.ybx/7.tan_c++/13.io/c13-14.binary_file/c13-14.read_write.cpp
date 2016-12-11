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
	int i;
	Student stud[3] = {"dabing", 111, 100,'m',
						"wc", 222, 99, 'm',
						"ly",333, 98, 'f'};
	ofstream outfile("stud.txt", ios::binary);
	if (!outfile) {
		cerr<<"open error"<<endl;
		exit(1);
	}
	//写法1:
	//for (i = 0; i < 3; i++) {
	//	outfile.write((char *) &stud[i], sizeof(stud[i]));
	//}
	//写法2:
	outfile.write((char *) stud, sizeof(stud));
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
	write_test();
	read_test();
	return 0;
}
