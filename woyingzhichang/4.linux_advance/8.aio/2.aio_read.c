#include <aio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>

#define FNAME "1.txt"
#define BUFSIZE 1024

int main(int argc, const char *argv[])
{
	int fd, ret;
	struct aiocb my_aiocb;

	fd = open( FNAME, O_RDONLY );
	if (fd < 0) perror("open");

	/* Zero out the aiocb structure (recommended) */
	bzero( (char *)&my_aiocb, sizeof(struct aiocb) );

	/* Allocate a data buffer for the aiocb request */
	my_aiocb.aio_buf = malloc(BUFSIZE+1);
	if (!my_aiocb.aio_buf) perror("malloc");

	/* Initialize the necessary fields in the aiocb */
	my_aiocb.aio_fildes = fd;
	my_aiocb.aio_nbytes = BUFSIZE;
	my_aiocb.aio_offset = 0;

	ret = aio_read( &my_aiocb );
	printf("aio_read:%d\n", ret);
	if (ret < 0) perror("aio_read");

	while ( aio_error( &my_aiocb ) == EINPROGRESS ){
		printf("EINPROGRESS!!\n");
		sleep(1);
	}

	if ((ret = aio_return( &my_aiocb )) > 0) {
		printf("%s\n", my_aiocb.aio_buf);
		printf("ret:%d\n", ret);
	} else {
		perror("aio_return");
	}


	return 0;
}


