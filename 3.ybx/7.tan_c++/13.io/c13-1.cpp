#include <iostream>
#include <unistd.h>
using namespace std;
int main(int argc, const char *argv[])
{
	cerr<<"cerr直接输出没有缓冲区";
	sleep(3);
	return 0;
}
