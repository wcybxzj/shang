#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

static void err_doit(const char *fmt, va_list ap){
	int errno_save, n;
	char buf[1024];

	errno_save = errno;

	vsnprintf(buf, sizeof(buf), fmt, ap);
	n = strlen(buf);
	snprintf(buf+n, sizeof(buf)-n, \
			": %s", strerror(errno_save));

	fprintf(stderr, "%s", buf);
	return;
}

//printf:让myprint按照printf进行类型检测
//1是指myprint第1个参数
//2是指myprint第2个参数
void myprint(const char *format,...) 
#ifdef __GNUC__ //如果是GNU c编译器也就是gcc ，要对myprint里...的参数进行类型检测
	__attribute__((format(printf,1,2)))
#endif
;
	
void myprint(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(fmt, ap);
	va_end(ap);
	return;
}

//gcc 1.format.c  -Wall
//printf 是可以对参数进行参数类型匹配测试的

//测试1:使用attribute 给myprint在编译时候报错
//通过__attribute__format 给自定义函数myprint增加和printf一样的

//测试2:不使用attribute 给myprint在编译时候不报错,执行时候报错
int main(int argc, const char *argv[])
{
	printf("%s\n",12313);
	myprint("i=%d\n",6);
	myprint("i=%s\n",6);
	myprint("i=%s\n","abc");
	myprint("%s,%d,%d\n",1,2);
	return 0;
}
