#include <stdio.h>
#include <string.h>


typedef struct student {
	int age;
	int sex;
	int tall;
}STU;

//问题:
//./39_2.struct_init 
//age:136827752, sex:32767, tall:4195701
void func1()
{
	STU stud1;
	printf("age:%d, sex:%d, tall:%d\n",stud1.age, stud1.sex, stud1.tall  );
}

//方法1:memeset或者bzero
//./39_2.struct_init 
//age:0, sex:0, tall:0
void func2()
{
	STU stud1;
	memset(&stud1, 0, sizeof(struct student));
	printf("age:%d, sex:%d, tall:%d\n",stud1.age, stud1.sex, stud1.tall  );
}

//方法2:
//./39_2.struct_init 
//age:0, sex:0, tall:0
void func3()
{
	STU stud1={0};
	printf("age:%d, sex:%d, tall:%d\n",stud1.age, stud1.sex, stud1.tall  );
}

//结构体数组的错误例子
//./39_2.struct_init 
//age:-806243032, sex:32767, tall:0
//age:1, sex:0, tall:0
//age:4195104, sex:0, tall:194
//age:0, sex:-806243074, tall:32767
//age:-806243073, sex:32767, tall:1
//age:0, sex:0, tall:0
//age:2066286093, sex:60, tall:0
//age:0, sex:4195984, tall:0
//age:0, sex:0, tall:4195291
//age:0, sex:-806242760, tall:32767
void func4()
{
	int i;
	STU stud_arr[10];//自动变量
	for (i = 0; i < 10; i++) {
		printf("age:%d, sex:%d, tall:%d\n",stud_arr[i].age, stud_arr[i].sex, stud_arr[i].tall  );
	}
}

//结构体数组用方法2初始化
//./39_2.struct_init 
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
//age:0, sex:0, tall:0
void func5()
{
	int i;
	STU stud_arr[10]={{0}};//用方法2来初始化结构数组
	for (i = 0; i < 10; i++) {
		printf("age:%d, sex:%d, tall:%d\n",stud_arr[i].age, stud_arr[i].sex, stud_arr[i].tall  );
	}
}

int main(int argc, const char *argv[])
{
	//func1();
	//func2();
	//func3();
	//func4();
	func5();
	return 0;
}
