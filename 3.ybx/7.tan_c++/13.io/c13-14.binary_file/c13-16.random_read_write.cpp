#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstring>
using namespace std;

struct Student
{
	char name[20];
	int num;
	int age;
	char sex;
};

int main(int argc, const char *argv[])
{
	int i;
	Student stud[5] = {
		"dabing", 111, 100,'m',
		"wc", 222, 99, 'm',
		"ly",333, 98, 'f',
		"ybx",444, 94, 'm',
		"fbb",555, 93, 'f',
	};
	fstream iofile("stud.txt", ios::in|ios::out|ios::binary);
	if (!iofile) {
		cerr<<"open error"<<endl;
		exit(1);
	}
	iofile.write((char *) stud, sizeof(stud));

	Student stud1[5];
	cout<<"=================================================="<<endl;
	for (i = 0; i < 5; i=i+2) {
		iofile.seekg(i*sizeof(stud[i]), ios::beg);
		iofile.read((char *) &stud1[i/2], sizeof(stud1[i]));
		cout<<"name:"<<stud1[i/2].name<<endl;
		cout<<"num:"<<stud1[i/2].num<<endl;
		cout<<"age:"<<stud1[i/2].age<<endl;
		cout<<"sex:"<<stud1[i/2].sex<<endl;
	}
	cout<<endl;

	strcpy(stud[2].name, "Diablo");
	stud[2].num = 1012;
	stud[2].age= 1000;
	stud[2].sex= 'm';

	iofile.seekp(2*sizeof(stud[0]), ios::beg);
	iofile.write((char*) &stud[2], sizeof(stud[2]));
	iofile.seekg(0, ios::beg);
	cout<<"=================================================="<<endl;
	for (i = 0; i < 5; i++) {
		iofile.read((char *) &stud[i], sizeof(stud[i]));
		cout<<"name:"<<stud1[i/2].name<<endl;
		cout<<"num:"<<stud1[i/2].num<<endl;
		cout<<"age:"<<stud1[i/2].age<<endl;
		cout<<"sex:"<<stud1[i/2].sex<<endl;
	}
	iofile.close();
	return 0;
}
