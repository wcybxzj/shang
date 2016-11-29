#include <iostream>
using namespace std;

class String
{
	public:
		String(){p=NULL;}
		String(char *str);
		void display();

	private:
		char *p;
};

String::String(char *str)
{
	p=str;
}

void String::display()
{
	cout<<p;
}

int main(int argc, const char *argv[])
{
	String string1("hello"), string2("Book");
	string1.display();
	cout<<endl;
	string2.display();
	cout<<endl;
	return 0;
}
