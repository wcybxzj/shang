#include <stdio.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

//http://blog.csdn.net/luotuo44/article/details/24933673#t6

//先来看一下命令行参数是怎么使用的。$ps -e、$find -name main.c这两个命令都使用了命令行参数。
//减号-表示一个命令行参数的开始，紧挨着-的是选项。
//像上面的e和name就是一个选项。其中选项e是没有参数值的，而选项name是有参数值的，为main.c。
//有一些选项既可以有参数值，也是可以没有参数值的。上面的-name选项就必须要有参数值。

//参数argc和argv是main函数的两个参数，而参数optstring指明要在argv中查找哪些选项。
//假如optstring的值为"ab:c:de::"，那么就说明会在argv中查找a、b、c、d、e这5个选项(不错，选项都是一个字符的)。
//其中b、c选项后面带有一个冒号:，说明b和c选项都要有一个参数。
//选项e后面有两个冒号，那么e选项既可以有参数也可以没有参数。选项a和d都没有冒号，说明这两个个选项

//可以看到，getopt函数的返回值就是对应选项的ASCII值。当查找完所有的命令行参数后就返回-1。
//如果一个选项是有参数的，那么全局变量optarg就会指向这个参数字符串的开始位置。
//从上面的代码也可以看到，如果选项e有参数，那么optarg就不等于NULL，否则就等于NULL。

//a.out -a -b 111 -c -d -e
//a.out -a -b 111 -c -d -e 222
int main(int argc, char** argv)  
{  
    int opt;  
  
    while( (opt = getopt(argc, argv, "ab:c:de::")) >= 0 )   
    {  
		if (opt=='b') {
		}
        switch( opt )  
        {  
            case 'a' :  
				printf("a\n");
				break;            
            case 'b' :  
				printf("option b has parm = %s\n", optarg); 
				break;  
            case 'c' :  
				printf("c\n");
						break;  
            case 'd' :  
				printf("d\n");
				break;  
  
            case 'e' :    
                    if(optarg)  
                    {  
                        printf("option e has parm\n");    
                    }  
                    break;  
  
            default : //出现了不是选项的字符  
					break;
        }  
    }  
}  
