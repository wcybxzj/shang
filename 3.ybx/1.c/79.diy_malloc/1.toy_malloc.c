#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/* 一个玩具malloc */
/*
这个malloc每次都在当前break的基础上增加size所指定的字节数，并将之前break的地址返回。这个malloc由于对所分配的内存缺乏记录，不便于内存释放，所以无法用于真实场景。
*/
void *toy_malloc(size_t size)
{
    void *p;
    p = sbrk(0);
    if (sbrk(size) == (void *)-1)
        return NULL;
    return p;
}


int main(int argc, const char *argv[])
{
	char *ptr=toy_malloc(10);
	strcpy(ptr,"123456789");
	printf("%s\n",ptr);
	return 0;
}
