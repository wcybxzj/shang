#include <iostream>
using namespace std;

class Complex
{
	public:
		Complex(double r){
			real = r;
			imag = 0;
		}

		void display();
	private:
		double real;
		double imag;
};

void Complex::display()
{
	cout<<"("<<real<<","<<imag<<"i)"<<endl;
}

int main(int argc, const char *argv[])
{
	Complex c1(3);
	c1.display();
	return 0;
}
