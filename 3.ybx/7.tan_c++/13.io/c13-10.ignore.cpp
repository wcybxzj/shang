#include <iostream>
using namespace std;

/*
./c13-10.ignore 
I like you/I hate you/
I like you
The current character ascii is:/
*/
void test1()
{
	char c;
	char ch[20];
	cin.get(ch, 20, '/');
	cout<<ch<<endl;
	c = cin.peek();//当前字符是'/'
	cout<<"The current character ascii is:"<<c<<endl;
	cin.get(ch, 20, '/');
	cout<<ch<<endl;
}

/*
I like you/I hate you/
I like you
The current character ascii is:/
I hate you
*/
void test2()
{
	char c;
	char ch[20];
	cin.get(ch, 20, '/');
	cout<<ch<<endl;
	c = cin.peek();//当前字符是'/'
	cout<<"The current character ascii is:"<<c<<endl;
	cin.ignore();//解决方法
	cin.get(ch, 20, '/');
	cout<<ch<<endl;
}

int main(int argc, const char *argv[])
{
	test1();
	//test2();
	return 0;
}
