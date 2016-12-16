#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

//证明1:十六进制保存的int,是如何保存
void test1()
{
	int a = 0x12345678;
	int ret = 1*pow(16,7)+ 2*pow(16,6)+ 3*pow(16,5)+ 4*pow(16,4)+
		5*pow(16,3)+ 6*pow(16,2)+ 7*pow(16,1)+8;
	printf("%d\n", a);//305419896
	printf("%d\n", ret);//305419896
}

//证明2: 因为是小端存储, 用char*指针只能保存整形的最后一个字节的信息
void test2()
{
	int a = 0x12345678;
	char *p = (char *) &a;
	printf("%d\n", *p);//120
	printf("%c\n", *p);//ascii 120 is 'c'
}

//证明3:
//1个char的范围是2的8次方256,
//acii范围是 0到127
//unsigned char范围是 0到255
void test3()
{
	int b = 0x01ff;
	unsigned char *p1 = (unsigned char *) &b;
	printf("%d\n", b);//511
	printf("%d\n", *p1);//255
	printf("%c\n", *p1);//非ascii中能对应的字符

	b = 0;
	p1 = (unsigned char *) &b;
	printf("%d\n", b);//0
	printf("%d\n", *p1);//0
	printf("%c\n", *p1);//换行

	b = 255;
	p1 = (unsigned char *) &b;
	printf("%d\n", b);//255
	printf("%d\n", *p1);//255
	printf("%c\n", *p1);//非ascii中能对应的字符
}

//证明4:
//char默认是有符号的
//signed char范围是 -128到127
//负数的2进制保存取得方式是取得补码存入
//例如-1先变成1,转成2进制所有位取反,+1
void test4()
{
	int b = 0x01ff;
	char *p2 = (char *) &b;
	printf("%d\n", b);//511
	printf("%d\n", *p2);//-1
	printf("%c\n", *p2);//非ascii中能对应的字符
	printf("===========================\n");

	b = 255;
	p2 = (char *) &b;
	printf("%d\n", b);//255
	printf("%d\n", *p2);//-1
	printf("%c\n", *p2);//非ascii中对应的字符

	printf("===========================\n");
	b = 0;
	p2 = (char *) &b;
	printf("%d\n", b);//0
	printf("%d\n", *p2);//0
	printf("%c\n", *p2);//换行

	printf("===========================\n");
	b = 127;
	p2 = (char *) &b;
	printf("%d\n", b);//127
	printf("%d\n", *p2);//127
	printf("%c\n", *p2);//ascii为127的ascii字符

	printf("===========================\n");
	b = 128;
	p2 = (char *) &b;
	printf("%d\n", b);//128
	printf("%d\n", *p2);//-128
	printf("%c\n", *p2);//

}

//测试5:
//模拟:10-1.unievent.c中 pipe中send， 强转成int 成 char *
//结论:char* 强转int,只能保存0-127的int
/*
./10-0.char\*_int 
i:0, var:0
i:1, var:127
i:2, var:-128
*/
static int pipefd[2];
void test5()
{
	int i;
	int ret;
	ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
	if (ret==-1) {
		perror("socketpair");
		exit(1);
	}

	pid_t pid;
	pid = fork();
	if (pid==0) {
		int var1 =0;
		int var2 =127;
		int var3 =128;
		send( pipefd[1], ( char* )&var1, 1, 0 );
		send( pipefd[1], ( char* )&var2, 1, 0 );
		send( pipefd[1], ( char* )&var3, 1, 0 );
	}else if(pid>0){
		sleep(1);//让子进程有时间去写
		char signals[1024];
		ret = recv(pipefd[0], signals, 1024, 0);
		for (i = 0; i < ret; i++) {
			printf("i:%d, var:%d\n", i,signals[i]);
		}
	}
}

int main(int argc, const char *argv[])
{
	//test1();
	//printf("==============\n");
	//test2();
	//printf("==============\n");
	//test3();
	//printf("==============\n");
	//test4();
	printf("==============\n");
	test5();
	return 0;
}
