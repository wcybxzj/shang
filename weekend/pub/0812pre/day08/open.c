#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int ret;

	ret = open("./exit", O_RDWR | O_CREAT, 0644);

	printf("ret = %d\n", ret);

	return 0;
}
