#include <stdio.h>

//循环版:查找特定位置的fibonacci值
int fib1(int n)
{
	int i, num=0;
	int p1=1, p2=1;
	if (n<=1) {
		return 1;
	}
	for (i = 2; i <= n; i++) {
		num=p1+p2;
		p1=p2;
		p2=num;
	}
	return num;
}

//递归版:查找特定位置的fibonacci值
int fib2(int n)
{
	if (n <= 1) {
		return 1;
	} else {
		return fib2(n - 1) +fib2(n - 2);
	}
}

//循环版打印
void print_fib1(int num)
{
	int i;
	int arr[100] = {1, 1};
	if (num<=0) {
		return;
	}
	if (num==1) {
		printf("%d\n",arr[0]);
	}else if(num==2){
		printf("%d %d\n",arr[0], arr[1]);
	}else{
		for (i = 2; i < num; i++) {
			arr[i] = arr[i-1] + arr[i-2];
		}
		for (i = 0; i < num; i++) {
			printf("%d ",arr[i]);
		}
		printf("\n");
	}
}

//递归版打印
void print_fib2(int num)
{
	int i;
	int arr[100] = {1, 1};
	if (num<=0) {
		return;
	}
	if (num==1) {
		printf("%d\n",arr[0]);
	}else if(num==2){
		printf("%d %d\n",arr[0], arr[1]);
	}else{
		for (i = 2; i < num; i++) {
			arr[i] = fib2(i);
		}
		for (i = 0; i < num; i++) {
			printf("%d ",arr[i]);
		}
		printf("\n");
	}
}


//1 1 2 3 5 8
int main(int argc, const char *argv[])
{
	int i;
	for (i = 0; i < 10; i++) {
		print_fib1(i);
		print_fib2(i);
	}
	return 0;
}
