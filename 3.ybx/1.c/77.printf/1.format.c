#include <stdio.h>
#include <string.h>

#define MAX 100

/*
[root@web11 www]# ./1
          ########
         x########
        xx########
          ########
x         ########
xx        ########
*/
void func()
{
	int i;
	for(i=0;i<3;i++)
		printf("%*.*s%s\n",10, i, "xx", "########");

	for(i=0;i<3;i++)
		printf("%-*.*s%s\n",10, i, "xx", "########");
}

/*
[root@web11 www]# ./1
abc                 |
                 abc|
*/
void func1()
{
int w=20;//宽度
int j=3;//精度
	int n;
	char buf[MAX],buf1[MAX];
	strcpy(buf, "abcdef");
	n = strlen(buf);
	snprintf(buf1, MAX, "%-*.*s%s",w, j, buf,"|");
	printf("%s\n", buf1);
	snprintf(buf1, MAX, "%*.*s%s",w, j, buf,"|");
	printf("%s\n", buf1);
}

//资料参考 C程序设计page135
int main(int argc, const char *argv[])
{
	//func();
	func1();
	return 0;
}
