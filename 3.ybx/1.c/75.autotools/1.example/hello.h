/*hello.h*/
#ifndef __HELLO_H__
#define __HELLO_H__

#include<iostream>
using namespace std;

class CHello
{
	public:
    CHello(){ cout<<"Hello!"<<endl;}
    ~CHello(){ cout<<"Bye!"<<endl;}
};

#endif
