#include <stdio.h>

#ifndef _EVENT_HAVE_FD_MASK
/* This type is mandatory, but Android doesn't define it. */
typedef unsigned long fd_mask;
#endif

#ifndef NFDBITS
#define NFDBITS (sizeof(fd_mask)*8)
#endif

/* Divide positive x by y, rounding up. */
#define DIV_ROUNDUP(x, y)   (((x)+((y)-1))/(y))

/* How many bytes to allocate for N fds? */
#define SELECT_ALLOC_SIZE(n) \
		(DIV_ROUNDUP(n, NFDBITS) * sizeof(fd_mask))


int main(int argc, const char *argv[])
{
	printf("%d\n", NFDBITS);//64

	printf("%d\n",SELECT_ALLOC_SIZE(1));//8
	printf("%d\n",SELECT_ALLOC_SIZE(64));//8
	printf("%d\n",SELECT_ALLOC_SIZE(65));//16
	printf("%d\n",SELECT_ALLOC_SIZE(128));//16

	return 0;
}
