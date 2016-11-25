#include <iostream>
using namespace std;


//使用默认参数的构造函数
class Box{
	public:
		Box(int w=10, int h=10, int len=10);
		int volume();
	private:
		int height;
		int width;
		int length;
};

Box::Box(int w, int h, int len)
{
	height=h;
	width=w;
	length=len;
}

int Box::volume()
{
	return(height*width*length);
}


int main(int argc, const char *argv[])
{
	Box box1;
	cout << "The volume of box1 is " << box1.volume()<< endl;

	Box box2(15, 30);
	cout << "The volume of box2 is " << box2.volume() << endl;

	Box box3(15, 20, 30);
	cout << "The volume of box3 is " << box3.volume()<< endl;
	return 0;
}
