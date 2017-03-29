#include "unp.h"
#include <time.h>


char *gf_time(void)
{
	char *ptr;
	char str[30];
	struct timeval tv;

	if (gettimeofday(&tv, NULL)<0) {
		perror("gettimeofday");
		exit(1);
	}

	ptr = ctime(&tv.tv_sec);
	//printf("%s\n", ptr);//Sun Mar 26 22:07:07 2017
	strcpy(str, &ptr[11]);//跳过前11个字符只要 时/分/秒
	//printf("%s\n", str);//22:07:07 2017
	snprintf(str+8, sizeof(str)-8, ".%06ld", tv.tv_usec);//追加usec到后边
	return str;
}

//int main(int argc, const char *argv[])
//{
//	gf_time();
//	return 0;
//}
