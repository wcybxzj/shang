#include <iostream>
using namespace std;

//class TV 对 class Tele 为友元
//Tele对象可以调用TV对象中的private属性
class TV
{
	public:
		friend class Tele;
		TV():on_off(off), mode(tv){}
	private:
		enum{on, off};
		enum{tv, av};
		bool on_off;
		int mode;
};

class Tele
{
	public:
		void OnOff(TV&t){
			t.on_off=(t.on_off==t.on)?t.off:t.on;
		}
		void SetMode(TV&t){
			t.mode=(t.mode==t.tv)?t.av:t.tv;
		}
		void show(TV&t) const;
};


void Tele::show(TV&t) const
{
	if (t.on_off == t.on) {
		cout<<"电视开启中"<<endl;
		if (t.mode == t.av) {
			cout<<"mode is av"<<endl;
		}else{
			cout<<"mode is av"<<endl;
		}
	}else{
		cout<<"电视关闭中"<<endl;
	}
}

int main(int argc, const char *argv[])
{
	Tele tele1;
	TV tv1;
	tele1.show(tv1);

	tele1.OnOff(tv1);
	tele1.show(tv1);

	return 0;
}
