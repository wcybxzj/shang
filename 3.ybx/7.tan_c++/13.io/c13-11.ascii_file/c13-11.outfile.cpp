#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

int main(int argc, const char *argv[])
{
	int i;
	int arr[10];

	//ofstream file("f1.txt", ios::out);
	ofstream file("f1.txt");
	if (!file) {
		cerr<<"open error!"<<endl;
		exit(1);
	}

	cout<<"enter 10 integer numbers:"<<endl;
	for (i = 0; i < 10; i++) {
		cin>>arr[i];
		file<<arr[i]<<" ";
	}
	file.close();
	return 0;
}
