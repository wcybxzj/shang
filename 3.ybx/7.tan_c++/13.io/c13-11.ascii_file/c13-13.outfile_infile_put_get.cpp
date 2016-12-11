#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

void save_to_file()
{
	ofstream outfile("f2.txt",ios::out);
	if (!outfile) {
		cerr<<"open error"<<endl;
		exit(1);
	}
	int i;
	char c[80];
	cin.getline(c, 80);
	for (i = 0; c[i]; i++) {
		if ((c[i]>=65 && c[i]<=90) || (c[i]>=97 && c[i]<=122)) {
			outfile.put(c[i]);
			cout<<c[i];
		}
	}
	cout<<endl;
	outfile.close();
}

void get_from_file()
{
	char ch;
	ifstream infile("f2.txt", ios::in);
	if (!infile) {
		cerr<<"open f2.txt"<<endl;
		exit(1);
	}

	ofstream outfile("f3.txt", ios::out);
	if (!outfile) {
		cout<<"f3.txt"<<endl;
		exit(1);
	}

	while (infile.get(ch)) {
		if (ch>=97 && ch<=122) {
			ch=ch-32;
		}
		outfile.put(ch);
		cout<<ch;
	}
	cout<<endl;
	infile.close();
	outfile.close();
}

int main(int argc, const char *argv[])
{
	save_to_file();
	get_from_file();
	return 0;
}
