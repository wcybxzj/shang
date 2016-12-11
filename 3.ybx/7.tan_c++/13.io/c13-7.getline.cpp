#include <iostream>
using namespace std;


//./c13-7.getline 
//123
//the second part is:123
//456
//the third part is:456
void test1()
{
	char ch[10];
	char ch1[10];

	cin.getline(ch, 10);
	cout<<"the second part is:"<<ch<<endl;

	cin.getline(ch1, 10);
	cout<<"the third part is:"<<ch1<<endl;
}

// ./c13-7.getline 
// 123/456789
// the second part is:123
// the third part is:456789
void test2()
{
	char ch[10];
	char ch1[10];

	cin.getline(ch, 10, '/');
	cout<<"the second part is:"<<ch<<endl;

	cin.getline(ch1, 10);
	cout<<"the third part is:"<<ch1<<endl;
}

//./c13-7.getline 
//123
//the second part is:123
//abcdefjhi
//the third part is:abcdefjhi
void test3()
{
	char ch[10];
	char ch1[10];

	cin.getline(ch, 10);
	cout<<"the second part is:"<<ch<<endl;

	cin.getline(ch1, 10, '/');
	cout<<"the third part is:"<<ch1<<endl;
}

//./c13-7.getline 
//123/456/
//the second part is:123
//the third part is:456
void test4()
{
	char ch[10];
	cin.getline(ch, 10, '/');
	cout<<"the second part is:"<<ch<<endl;
	cin.getline(ch, 10, '/');
	cout<<"the third part is:"<<ch<<endl;
}

int main(int argc, const char *argv[])
{
	//test1();
	//test2();
	//test3();
	test4();
	return 0;
}
