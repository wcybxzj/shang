#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;

int main(int argc, const char *argv[])
{
	int i, max, order;
	int arr[10];
	ifstream infile("f1.txt", ios::in);
	if (!infile) {
		cerr<<"open error!"<<endl;
		exit(1);
	}

	for (i = 0; i < 10; i++) {
		infile>>arr[i];
		cout<<arr[i]<<" ";
	}
	cout<<endl;
	max = arr[0];
	order = 0;

	for (i = 1; i < 10; i++) {
		if (arr[i]>max) {
			max=arr[i];
			order = i;
		}
	}

	cout<<"max="<<max<<endl;
	cout<<"order="<<order<<endl;
	infile.close();

	return 0;
}
