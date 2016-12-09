#include <iostream>
#include <string>
using namespace std;


//抽象基类
class Shape
{
	public:
		virtual float area() const {return 0.0;} //虚函数
		virtual float volume() const {return 0.0;} //虚函数
		virtual void shapeName() const =0; //纯虚函数
};

class Point:public Shape
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
		virtual void shapeName() const {cout<<"Point"<<endl;}
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
		virtual void shapeName() const {cout<<"Circle"<<endl;}
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
		virtual void shapeName() const {cout<<"Cylinder"<<endl;}
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
	Point p(1.1, 2.2);
	Circle c(3.3, 4.4, 5.5);
	Cylinder cy(5, 6, 7, 8);

	p.shapeName();
	cout<<p<<endl;

	c.shapeName();
	cout<<c<<endl;

	cy.shapeName();
	cout<<cy<<endl;
	cout<<"========================"<<endl;

	Shape *pt;
	pt = &p;
	pt->shapeName();

	cout<<p.getX()<<endl;
	cout<<p.getY()<<endl;
	cout<<pt->area()<<endl;
	cout<<pt->volume()<<endl;

	pt = &c;
	pt->shapeName();
	cout<<c.getX()<<endl;
	cout<<c.getY()<<endl;
	cout<<pt->area()<<endl;
	cout<<pt->volume()<<endl;

	pt = &cy;
	pt->shapeName();
	cout<<cy.getX()<<endl;
	cout<<cy.getY()<<endl;
	cout<<pt->area()<<endl;
	cout<<pt->volume()<<endl;

	return 0;
}
