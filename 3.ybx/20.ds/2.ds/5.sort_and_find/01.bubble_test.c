#include <time.h>
#define  NUM 20


//3个冒泡版本的测试
//体现出正版冒泡bubble_version1()可以优化bubble_version2()
/*
比较次数:45 移动次数:9
遍历:
11 22 33 44 55 66 77 88 99 100

比较次数:45 移动次数:9
遍历:
11 22 33 44 55 66 77 88 99 100

比较次数:17 移动次数:9
遍历:
11 22 33 44 55 66 77 88 99 100
*/
void test1_bubble()
{
	int i=0;
	int num=0;
	int arr2[]  = {100,11,22,33,44,55,66,77,88,99};
	int arr3[]  = {100,11,22,33,44,55,66,77,88,99};
	int arr4[]  = {100,11,22,33,44,55,66,77,88,99};
	num =sizeof(arr2)/sizeof(*arr2);

	bubble_version0(arr2, num);
	travel(arr2, num);

	bubble_version1(arr3, num);
	travel(arr3, num);

	bubble_version2(arr4, num);
	travel(arr4, num);
}



int main(int argc, const char *argv[])
{
	test1_bubble();
	return 0;
}
