#include <iostream>
using namespace std;

int main(int argc, const char *argv[])
{
	char c;
	while (!cin.eof()) {
		if ((c = cin.get())!=' ') {
			cout.put(c);
		}
	}
	return 0;
}
