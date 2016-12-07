#include <iostream>
#include <string>
using namespace std;


int main(int argc, const char *argv[])
{
	string s1;
	char s2[10]="ok";
	char *s3 = "yes";
	s1 = s2;
	cout<<s1<<endl;

	s1= s3;
	cout<<s1<<endl;

	return 0;
}
