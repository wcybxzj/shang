#include <stdio.h>

//void func(int *p, int n)
void func(int p[], int n)
{
	//就因为这不是20,　所以必须传递数组长度
	printf("%s:%d\n",__FUNCTION__, sizeof(p));//32位 4
	int i;
	for (i = 0; i < n; i++) {
		//printf("%d\n", p[i]);
		printf("%d\n", *(p+i));
	}
}


int main(int argc, const char *argv[])
{
	int arr[] = {1, 3, 5, 7, 9};
	printf("%s:%d\n", __FUNCTION__, sizeof(arr));//20
	func(arr, sizeof(arr)/sizeof(*arr));
	return 0;
}
