#include <iostream>
#include <string.h>
using namespace std;

void test1()
{
	string str1;
	string str2 = "China";

	str1 = "yangbingxi";
	str2 = str1;
	cout << str1 << endl;
}

void test2()
{
	string str1;
	cin >> str1;
	cout << str1 << endl;
}

void test3()
{
	string str1, str2;
	str1 = "this is an ";
	str2 = "apple!";
	str1 = str1+str2;
	cout << str1 << endl;
}


void test4()
{
	string str1, str2;
	str1 = "this is an ";
	str2 = "apple!";
	if (str1 > str2) {
		cout << str1 << endl;
	}else{
		cout << str2 << endl;
	}
}

int main(int argc, const char *argv[])
{
	//test1();
	//test2();
	//test3();
	test4();
	return 0;
}
