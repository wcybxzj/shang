#include <strstream>
#include <iostream>
using namespace std;

struct student
{
	int num;
	char name[20];
	float score;
};


int main(int argc, const char *argv[])
{
	student stud[3]=
	{
		1001,"ybx", 78,
		1002, "wc", 88,
		1003, "ly", 98
	};
	int i;
	char arr[50];
	ostrstream strout(arr, 30);
	for (i = 0; i < 3; i++) {
		strout<<stud[i].num<<stud[i].name<<stud[i].score<<" ";
	}
	strout<<ends;
	cout<<"arr:"<<endl<<arr<<endl;
	return 0;
}
