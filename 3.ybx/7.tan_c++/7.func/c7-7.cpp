#include <iostream>
using namespace std;

int main(int argc, const char *argv[])
{
	enum color {red, yellow, blue, green, white, black};
	color pri;
	int zero = 0;
	int one = 1;
	int three = 3;
	pri = yellow;

	if (one == pri) {
		pri = color(three);
	}
	cout << pri << endl;
	return 0;
}
