#include <iostream>
#include <string>
using namespace std;
typedef struct{
	string name;
	int age;
	float score;
} STU;
//函数模板
template<class T> const T& Max(const T& a, const T& b);
//函数模板的显示具体化（针对STU类型的显示具体化）
template<> const STU& Max<STU>(const STU& a, const STU& b);
//重载<<
ostream & operator<<(ostream &out, const STU &stu);

/*
20
王明 , 16 , 95.5
*/
int main(){
	int a = 10;
	int b = 20;
	cout<<Max(a, b)<<endl;

	STU stu1 = { "王明", 16, 95.5};
	STU stu2 = { "徐亮", 17, 90.0};
	cout<<Max(stu1, stu2)<<endl;
	return 0;
}
template<class T> const T& Max(const T& a, const T& b){
	return a > b ? a : b;
}

template<> const STU& Max<STU>(const STU& a, const STU& b){
	return a.score > b.score ? a : b;
}

ostream & operator<<(ostream &out, const STU &stu){
	out<<stu.name<<" , "<<stu.age <<" , "<<stu.score;
	return out;
}
