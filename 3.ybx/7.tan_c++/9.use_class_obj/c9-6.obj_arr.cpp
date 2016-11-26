#include <iostream>
using namespace std;

class Box{
	public:
		Box(int h=10, int w=12, int len=15): \
			height(h), width(w), length(len){}
		int volume();
	private:
		int height;
		int width;
		int length;
};

int Box::volume()
{
	return (height*width*length);
}

int main(int argc, const char *argv[])
{
	Box arr[3] = {
		Box(10, 12, 15),
		Box(10, 13, 15),
		Box(10, 14, 15)
	};
	
	cout << "volume of arr[0] is "<< arr[0].volume() << endl;
	cout << "volume of arr[1] is "<< arr[1].volume() << endl;
	cout << "volume of arr[2] is "<< arr[2].volume() << endl;

	return 0;
}
