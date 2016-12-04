#include <iostream>
using namespace std;

class Complex
{
	public:
		friend istream& operator>>(istream&, Complex &);
		friend ostream& operator<<(ostream&, Complex &);
	private:
		double real;
		double imag;
};

istream& operator>>(istream& input, Complex& c)
{
	input>>c.real>>c.imag;
	//return input;
	return cin; 
}

ostream& operator<<(ostream& output, Complex& c)
{
	output<<c.real<<":"<<c.imag;
	//return output;
	return cout;
}

int main(int argc, const char *argv[])
{
	Complex c1, c2;
	cin>>c1>>c2;
	cout<<c1<<endl;
	cout<<c2<<endl;
	return 0;
}
