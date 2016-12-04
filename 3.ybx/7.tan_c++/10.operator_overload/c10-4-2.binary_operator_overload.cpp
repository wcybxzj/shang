#include <iostream>
#include <cstring>
using namespace std;

//二进制运算符重载
class String
{
	public:
		String(){p=NULL;}
		String(char *str);
		void display();
		friend bool operator > (String &string1, String &string2);
		friend bool operator < (String &string1, String &string2);
		friend bool operator == (String &string1, String &string2);
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

bool operator > (String &string1, String &string2)
{
	if (strcmp(string1.p, string2.p)>0) {
		return true;
	}else{
		return false;
	}
}

bool operator < (String &string1, String &string2)
{
	if (strcmp(string1.p, string2.p)<0) {
		return true;
	}else{
		return false;
	}
}

bool operator == (String &string1, String &string2)
{
	if (strcmp(string1.p, string2.p)==0) {
		return true;
	}else{
		return false;
	}
}

void compare(String &string1, String &string2)
{
	if (operator>(string1, string2)==1) {
		string1.display();
		cout<<">";
		string2.display();
	}else{
		if (operator==(string1, string2)==1) {
			string1.display();
			cout<<"==";
			string2.display();
		}else{
			string1.display();
			cout<<"<";
			string2.display();
		}
	}


}
int main(int argc, const char *argv[])
{
	String string1("hello"), string2("Book");
	String string3("nice"), string4("see");

	cout<<(string1>string2)<<endl;
	cout<<(string1<string2)<<endl;
	cout<<(string1==string2)<<endl;
	cout<<endl;

	compare(string1, string2);
	compare(string2, string3);
	compare(string1, string4);
	cout<<endl;

	return 0;
}
