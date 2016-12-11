#include <iostream>
#include <stdio.h>
using namespace std;

void c_plus_test()
{
	char c;
	cout<<"entrer a sentence:"<<endl;
	while ((c = cin.get()) !=EOF)  {
		cout.put(c);
	}
}

void c_test()
{
	char c;
	printf("entrer a sentence:\n");
	while ((c=getc(stdin)) !=EOF)  {
		putc(c, stdout);
	}
}

//ctrl+D 是EOF
int main(int argc, const char *argv[])
{
	//c_plus_test();
	c_test();
	return 0;
}
