#include <iostream>
using namespace std;

/*
./c13-9.peek_putback 
plz enter a sentence:
I am yeye/ are baba/
I am yeye
The nex character ascii is:32
I are baba
*/
int main(int argc, const char *argv[])
{
	char c[20];
	int ch;
	cout<<"plz enter a sentence:"<<endl;
	cin.getline(c, 15, '/');
	cout<<c<<endl;
	ch=cin.peek();//返回当前指针指向的当前是 '空格'
	cout<<"The current character ascii is:"<<ch<<endl;
	cin.putback(c[0]);//'I' 当前流第一个位置 替代 '空格'
	cin.getline(c, 15, '/');
	cout<<c<<endl;
	return 0;
}
