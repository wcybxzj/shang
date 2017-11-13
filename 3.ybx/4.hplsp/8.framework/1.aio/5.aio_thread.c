#include <aio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <strings.h>

#define FNAME "1.txt"
#define BUF_SIZE 4096
#define NBUF 10

void* aio_completion_handler (sigval_t sigval)
{
	int ret;
	struct aiocb *req;
	req = (struct aiocb *)sigval.sival_ptr;
	ret =  aio_return(req);
	if (ret > 0) {
		printf("%s", req->aio_buf);
		fflush(NULL);
	} else {
		perror("aio_return");
	}
	return;
}

void setup_io(int fd)
{
	int ret;
	struct sigaction sig_act;
	struct aiocb my_aiocb;
	off_t off = 0;
	struct stat sbuf;
	const struct aiocb *aiolist[NBUF];

	if (fstat(fd, &sbuf)<0) {
		perror("fstat");
		exit(1);
	}

	/* Set up the AIO request */
	bzero( (char *)&my_aiocb, sizeof(struct aiocb) );
	my_aiocb.aio_fildes = fd;
	my_aiocb.aio_buf = malloc(BUF_SIZE+1);
	my_aiocb.aio_nbytes = BUF_SIZE;

	/* Link the AIO request with a thread callback */
	my_aiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;
	my_aiocb.aio_sigevent.sigev_notify_function = (void*)aio_completion_handler;
	my_aiocb.aio_sigevent.sigev_notify_attributes = NULL;
	my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;


	while (off < sbuf.st_size) {
		bzero((char *)my_aiocb.aio_buf, BUF_SIZE+1); 
		my_aiocb.aio_offset = off;
		off+=BUF_SIZE;
		ret = aio_read( &my_aiocb );
		if (ret < 0) {
			perror("aio_read");
			exit(1);
		}

		//业务代码
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);

		//方法1:
		ret =  aio_error(&my_aiocb);
		if(ret == EINPROGRESS ){
			sleep(1);
		}


	}
	return;
}

int main(int argc, const char *argv[])
{
	int fd, ret;
	struct aiocb my_aiocb;

	fd = open( FNAME, O_RDONLY );
	if (fd < 0) perror("open");

	setup_io(fd);

	pause();

	return 0;
}
