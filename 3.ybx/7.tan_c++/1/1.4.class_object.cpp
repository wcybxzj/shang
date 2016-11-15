#include <iostream>
using namespace std;

class Student {
	private:
		int num;
		int score;
	public:
		void setdata(){
			cin>>num;
			cin>>score;
		};

		void display()
		{
			cout<<"num="<<num<<endl;
			cout<<"score="<<score<<endl;
		};
};



int main(int argc, const char *argv[])
{
	Student s1, s2;
	s1.setdata();
	s1.display();

	s2.setdata();
	s2.display();
	return 0;
}
