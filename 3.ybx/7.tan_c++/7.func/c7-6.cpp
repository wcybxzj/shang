#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
using namespace  std;

struct Student{
	string name;
	int num;
	char sex;
};

int main(int argc, const char *argv[])
{
	Student *p;
	p = new Student;
	if (p == NULL) {
		perror("new");
		exit(0);
	}
	p->name = "DaBing";
	p->num = 123;
	p->sex = 'm';
	cout << p->name << endl;
	cout << p->num << endl;
	cout << p->sex << endl;
	delete p;
	return 0;
}
