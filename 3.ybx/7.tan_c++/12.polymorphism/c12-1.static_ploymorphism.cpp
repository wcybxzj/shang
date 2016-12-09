#include <iostream>
#include <string>
using namespace std;

//静态多态例子
class Point
{
	protected:
		float x;
		float y;
	public:
		Point(float a=0, float b=0);
		void setPoint(float a, float b);
		float getX() const {
			return x;
		}
		float getY() const {
			return y;
		}
		friend ostream & operator<<(ostream &, const Point &);
};


Point::Point(float a, float b)
{
	x = a;
	y = b;
}

void Point::setPoint(float a, float b)
{
	x = a;
	y = b;
}

ostream & operator<<(ostream &output, const Point &p)
{
	output<<"x:"<<p.x<<"y:"<<p.y<<endl;
	return output;
}

class Circle:public Point
{
	protected:
		float radius;
	public:
		Circle(float x=0, float y=0, float r=0);
		void setRadius(float);
		float getRadius() const;
		float area() const;
		friend ostream & operator<<(ostream & output, const Circle &c);
};

Circle::Circle(float a, float b, float r):Point(a, b),radius(r){}

void Circle::setRadius(float r)
{
	radius = r;
}

float Circle::getRadius() const
{
	return radius;
}

float Circle::area() const
{
	return 3.14*radius*radius;
}

ostream & operator<<(ostream & output, const Circle &c)
{
	output<<"x:"<<c.x<<"y:"<<c.y<<endl;
	output<<"r:"<<c.radius<<endl;
	output<<"area:"<<c.area()<<endl;
}

class Cylinder:public Circle{
	protected:
		float height;

	public:
		Cylinder(float x=0, float y=0, float r=0, float h=0);
		void setHeight(float);
		float getHeight();
		float area() const;
		float volume() const;
		friend ostream & operator<<(ostream &, const Cylinder&);
};

Cylinder::Cylinder(float x, float y, float r, float h):Circle(x, y, r), height(h){}

void Cylinder::setHeight(float h)
{
	height = h;
}

float Cylinder::getHeight()
{
	return height;
}

float Cylinder::area() const
{
	return 2*Circle::area()+2*3.14*radius*height;
}

float Cylinder::volume() const
{
	return Circle::area()*height;
}

ostream & operator<<(ostream & output, const Cylinder& c)
{
	output<<"x:"<<c.x<<"y:"<<c.y<<endl;
	output<<"r:"<<c.radius<<endl;
	output<<"h:"<<c.height<<endl;
	output<<"area:"<<c.area()<<endl;
	output<<"vol:"<<c.volume()<<endl;
}

int main(int argc, const char *argv[])
{
	Cylinder cy1(5, 6, 7, 8);
	cout<<cy1.getX()<<endl;
	cout<<cy1.getY()<<endl;
	cout<<cy1.getRadius()<<endl;
	cout<<cy1.getHeight()<<endl;
	cout<<cy1.area()<<endl;
	cout<<cy1.volume()<<endl;

	cy1.setPoint(111,222);
	cy1.setRadius(7.7);
	cy1.setHeight(9.9);

	cout<<cy1<<endl;
	Point &p = cy1;
	cout<<p<<endl;
	Circle &cp = cy1;
	cout<<cp<<endl;

	return 0;
}
