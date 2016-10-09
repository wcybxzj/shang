#include <aio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#define FNAME1 "1.txt"
#define FNAME2 "2.txt"
#define BUFSIZE 1024
#define MAX_LIST 2

//没实现
//_SC_AIO_LISTIO_MAX设置不进去
int main(int argc, const char *argv[])
{
	struct aiocb aiocb1, aiocb2;
	struct aiocb * list[MAX_LIST];
	int fd1, fd2, ret;

	fd1 = open( FNAME1, O_RDONLY );
	if (fd1 < 0) perror("open");

	fd2 = open( FNAME2, O_RDONLY );
	if (fd2 < 0) perror("open");

	aiocb1.aio_fildes = fd1;
	aiocb1.aio_buf = malloc( BUFSIZE+1 );
	aiocb1.aio_nbytes = BUFSIZE;
	aiocb1.aio_offset = 0;
	aiocb1.aio_lio_opcode = LIO_READ;

	aiocb2.aio_fildes = fd2;
	aiocb2.aio_buf = malloc( BUFSIZE+1 );
	aiocb2.aio_nbytes = BUFSIZE;
	aiocb2.aio_offset = 0;
	aiocb2.aio_lio_opcode = LIO_WRITE;

	bzero( (char *)list, sizeof(list) );
	list[0] = &aiocb1;
	list[1] = &aiocb2;

	//int num;
	//num = sysconf(_SC_AIO_LISTIO_MAX);
	//if (num == -1) {
	//	perror("sysconf()");
	//	exit(1);
	//}

	struct rlimit *r_val = malloc(sizeof(struct rlimit));
	r_val->rlim_cur=2;
	r_val->rlim_max=4;
	ret =  setrlimit(_SC_AIO_LISTIO_MAX, r_val);
	//if (ret==-1) {
	//	perror("setrlimit()");
	//	exit(1);
	//}

	ret = lio_listio( LIO_NOWAIT, list, 2, NULL);
	if (ret==0) {
		printf("ok\n");
	}else{
		perror("lio_listio()");
	}
	printf("111111111111\n");
	return 0;
}
