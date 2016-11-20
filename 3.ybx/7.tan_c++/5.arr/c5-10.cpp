#include <iostream>
#include <cstring>
using namespace std;

void max_string(char arr[][30], int n)
{
	int i;
	char str[30];
	strcpy(str, arr[0]);
	for (i = 0; i < n; i++) {
		if (strcmp(str, arr[i]) > 0) {
			strcpy(str, arr[i]);
		}
	}
	cout << endl <<"the largest string is:" << str <<endl;
}

int main(int argc, const char *argv[])
{
	int i;
	char arr[3][30];
	for (i = 0; i < 3; i++) {
		cin >> arr[i];
	}
	max_string(arr, 3);
	return 0;
}
