#include <iostream>
using namespace std;

//对象的复制-自定义复制构造函数:
class Box{
	public:
		Box(const Box &b);
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

//自定义复制构造函数
Box::Box(const Box &b)
{
	cout <<"custom constructor function" <<endl;
	height=b.height;
	width=b.width;
	length=b.length;
}

int Box::volume()
{
	return (height*width*length);
}

//测试1:复制形式1
void test1()
{
	Box box1(15, 30, 25);
	Box box2(box1);
	cout << "the volume of box1 is "<<box1.volume()<<endl;
	cout << "the volume of box2 is "<<box2.volume()<<endl;
}

//测试2:复制形式2
void test2()
{
	Box box1(15, 30, 25);
	Box box2 = box1;
	cout << "the volume of box1 is "<<box1.volume()<<endl;
	cout << "the volume of box2 is "<<box2.volume()<<endl;
}

//结果:
//constructor work!
//custom constructor function
//the volume of box1 is 11250
//the volume of box2 is 11250
//================
//constructor work!
//custom constructor function
//the volume of box1 is 11250
//the volume of box2 is 11250
int main(int argc, const char *argv[])
{
	test1();
	cout << "================" <<endl;
	test2();
	return 0;
}
