#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;
void display_file(const char *filename)
{
	ifstream infile(filename, ios::in);
	if (!infile) {
		cerr<<"open error"<<endl;
		exit(1);
	}
	char ch;
	while (infile.get(ch)) {
		cout.put(ch);
	}
	cout<<endl;
	infile.close();
}


int main(int argc, const char *argv[])
{
	display_file("f3.txt");
	return 0;
}
