#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


//http://blog.csdn.net/luotuo44/article/details/24933673#t6

//getsubopt：
//getsubopt函数一般是和getopt函数配合使用的，用来处理某一个选项的参数。
//当一个选项的参数比较复杂时，就可以使用getsubopt函数。
//-o表示一个选项。 而后面的那些都是该选项的参数，挺复杂的。
//选项o有三个子选项ro、rw和name，而只有name有参数值main.c。这些子选项之间用逗号分开，不能用空格。


//./2.getsubopt -o ro,rw,name=main.c
int main(int argc, char **argv)  
{  
	enum {  
		RO_OPT = 0,  
		RW_OPT,  
		NAME_OPT  
	};  
	char *const token[] = {  
		[RO_OPT]   = "ro",  
		[RW_OPT]   = "rw",  
		[NAME_OPT] = "name",  
		NULL  
	};  
	char *subopts;  
	char *value;  
	int err = 0;  
	int opt;  

	while ((opt = getopt(argc, argv, "aeo:")) != -1) {  
		switch (opt) {  
			case 'a': break;  
			case 'e': break;  
			case 'o':  
					  subopts = optarg;//optarg指向参数字符串的开始位置  
					  while (*subopts != '\0' && !err) {  
						  //getsubopt会修改subopts的值  
						  switch (getsubopt(&subopts, token, &value)) {  
							  case RO_OPT:  
								  if( value )  
									  printf("ro parm = %s\n", value);  
								  else  
									  printf("ro\n");  
								  break;  

							  case RW_OPT:  
								  if( value )  
									  printf("rw parm = %s\n", value);  
								  else  
									  printf("rw\n");  
								  break;  
								  break;  

							  case NAME_OPT:  
								  if( value )  
									  printf("name parm = %s\n", value);  
								  else  
									  printf("name\n");  
								  break;  

							  default:  
								  err = 1;  
								  break;  
						  }  
					  }  
		}  
	}  

	return 0;  
}  
