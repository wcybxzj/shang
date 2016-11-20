#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, const char *argv[])
{
	int i;
	int arr[20] = {1, 1};

	for (i = 2; i < 20; i++) {
		arr[i]=arr[i-2]+arr[i-1];
	}

	for (i = 0; i < 20; i++) {
		if (i%5==0) {
			cout<<endl;
		}
		cout<<setw(5)<<arr[i];
	}
	cout<<endl;
	return 0;
}
