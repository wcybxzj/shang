#include <iostream>
using namespace std;
template<class numtype>
class Compare{
	public:
		Compare(numtype a, numtype b)
		{
			x=a;
			y=b;
		}
		numtype max();

		numtype min()
		{
			return (x<y)?x:y;
		}
	private:
		numtype x, y;
};

//在模板外定义函数
template<class numtype>
numtype Compare <numtype> :: max()
{
	return (x>y)?x:y;
}

int main(int argc, const char *argv[])
{
	Compare<int> cmp1(3, 7);
	cout<<cmp1.max()<<endl;
	cout<<cmp1.min()<<endl;

	Compare<float> cmp2(1.3, 2.7);
	cout<<cmp2.max()<<endl;
	cout<<cmp2.min()<<endl;
	return 0;
}
