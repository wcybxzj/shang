#include <iostream>
using namespace std;

//对象的复制-默认复制构造函数:
//1.对象复制是新创建出一个新对象
//2.复制过程中调用复制构造函数
//3.复制构造函数可以自定义，如果不自定义默认是复制当前对象所有数据成员
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

//测试1:复制形式1
//结果:
//constructor work!
//the volume of box1 is 11250
//the volume of box2 is 11250
void test1()
{
	Box box1(15, 30, 25);
	Box box2(box1);
	cout << "the volume of box1 is "<<box1.volume()<<endl;
	cout << "the volume of box2 is "<<box2.volume()<<endl;
}

//测试2:复制形式2
//结果:
//constructor work!
//the volume of box1 is 11250
//the volume of box2 is 11250
void test2()
{
	Box box1(15, 30, 25);
	Box box2 = box1;
	cout << "the volume of box1 is "<<box1.volume()<<endl;
	cout << "the volume of box2 is "<<box2.volume()<<endl;
}

int main(int argc, const char *argv[])
{
	test1();
	cout << "========" <<endl;
	test2();
	return 0;
}
