#include <iostream>
using namespace std;

//第二种初始化方式,参数初始化表
class Box{
	public:
		Box();
		Box(int h, int w, int l): \
			height(h), width(w), length(l){}
		int volume();

	private:
		int height;
		int width;
		int length;
};

//构造函数重构
Box::Box(){
	height = 11;
	width = 12;
	length = 13;
}

int Box::volume(){
	return (height*width*length);
}

int main(int argc, const char *argv[])
{
	Box box1;
	cout << "The volume of box1 is " << box1.volume() << endl;

	Box box2(11, 22, 33);
	cout << "The volume of box2 is " << box2.volume()<<endl;
	return 0;
}
