#include <iostream>
using namespace std;

class Complex
{
	public:

		Complex(){
			real=0;
			imag=0;
		}

		Complex(double r, double i){
			real = r;
			imag = i;
		}

		friend Complex operator+(Complex &c1, double d1);
		void display(){
			cout<<real<<":"<<imag<<endl;
		}
	private:
		double real;
		double imag;
};

Complex operator+(Complex &c1, double d1){
	Complex c3;
	c3.real = c1.real+d1;
	c3.imag = 0;
	return c3;
}

class Complex1
{
	public:
		Complex1(){
			real=0;
			imag=0;
		}

		Complex1(double r, double i){
			real = r;
			imag = i;
		}

		operator double(){return real;}
	private:
		double real;
		double imag;
};

void test()
{
	Complex c1(3, 4),c2;
	c2 = c1+2.5;//3+2.5
	c2.display();
}

void test1()
{
	Complex1 c1(3, 4);
	double d;
	d = c1+2.5;
	cout<<d<<endl;
}
int main(int argc, const char *argv[])
{
	test();
	cout<<"============"<<endl;
	test1();
	return 0;
}
