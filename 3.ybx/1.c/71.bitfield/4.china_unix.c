#include <stdio.h>
//http://www.cnblogs.com/pure/archive/2013/04/22/3034818.html
void main(int argn ,char *argv)
{
    struct     test {
        unsigned a:10;
        unsigned b:10;
        unsigned c:6;
        unsigned :2;//this two bytes can't use
        unsigned d:4;
	}data,*pData;
    data.a=0x177;
    data.b=0x111;
    data.c=0x7;
    data.d=0x8;
    
    pData=&data;
    printf("data.a=%x data.b= %x data.c=%x data.d=%x\nn", 
		pData->a,pData->b,pData->c,pData->d);//位域可以使用指针

    printf("sizeof(data)=%d\n",sizeof(data));   //4 bytes ，最常用的情况

    struct testLen{
		char a:5;
		char b:5;
		char c:5;
		char d:5;
		char e:5;
    }len;
    
    printf("sizeof(len)=%d\n",sizeof(len));     //5bytes 规则2

    struct testLen1{
        char a:5;
        char b:2;
        char d:3;
        char c:2;
        char e:7;
        }len1;
    printf("sizeof(len1) =%d\n",sizeof(len1));    //3bytes 规则1


	printf("long:%d\n", sizeof(long));
    struct testLen2{
        char a:2;
        char :3;
        char b:7;
        long d:20; //4bytes
        char e:4;
        }len2;
    printf("sizeof(len2)=%d\n",sizeof(len2));  //8

    struct testLen3{
        char a:2;
        char :3;
        char b:7;
        long d:60;
        char e:4;
        }len3;
    printf("sizeof(len3)=%d\n",sizeof(len3));//16
}
