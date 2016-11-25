#include <iostream>
using namespace std;

class Time{
	private:
		int hour;
		int minute;
		int sec;
	public:
		void set(){
			cin  >> hour >> minute >> sec;
		}
};


int main(int argc, const char *argv[])
{
	Time t1;
	cout << sizeof(t1);

	return 0;
}
