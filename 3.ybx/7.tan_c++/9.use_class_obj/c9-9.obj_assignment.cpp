#include <iostream>
using namespace std;

//对象的赋值:
//将一个对象中的数据成员赋值给另外一个已经存在的对象
class Box{
	public:
		Box(int h =10, int w=10, int l=10);
		int volume();
	private:
		int height;
		int width;
		int length;
};

Box::Box(int h, int w, int l)
{
	cout << "constructor work!"<<endl;
	height=h;
	width=w;
	length=l;
}

int Box::volume()
{
	return (height*width*length);
}

//constructor work!
//constructor work!
//the volume of box1 is 11250
//the volume of box2 is 11250
int main(int argc, const char *argv[])
{
	Box box1(15, 30, 25), box2;
	cout << "the volume of box1 is "<<box1.volume()<<endl;
	box2 = box1;
	cout << "the volume of box2 is "<<box2.volume()<<endl;
	return 0;
}
