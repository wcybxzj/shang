#include <iostream>
using namespace std;
class Point
{
	public:
		Point(){}
		virtual ~Point(){
			cout<<"Point destructor"<<endl;
		}
};

class Circle:public Point
{
	public:
		Circle(){}
		~Circle(){
			cout<<"Circle destructor"<<endl;
		}
	private:
		int radius;
};

// ./c12-3.destructor 
// Circle destructor
// Point destructor
int main(int argc, const char *argv[])
{
	Point *p = new Circle;
	delete p;
	return 0;
}
