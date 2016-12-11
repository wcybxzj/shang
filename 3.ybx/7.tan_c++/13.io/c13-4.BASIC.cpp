#include <iostream>
#include <stdio.h>
using namespace std;

int main(int argc, const char *argv[])
{
	int i;
	const char *p ="BASIC";
	for (i = 4; i >=0; i--) {
		//cout<<*(p+i);
		putchar(*(p+i));
	}
	cout<<endl;
	return 0;
}
