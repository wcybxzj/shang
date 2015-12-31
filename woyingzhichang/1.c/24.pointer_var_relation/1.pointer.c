#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i=1;
	int *p = &i;

	printf("sizeof(i):%d\n",sizeof(i));//4
	printf("sizeof(p):%d\n",sizeof(p));//32位指针４字节,64位指针8字节

	printf("i = %d\n", i);
	printf("&i = %p\n", &i);
	printf("p = %p\n", p);//p保存的是i的地址所以%p
	printf("&p = %p\n", &p);



	return 0;
}
