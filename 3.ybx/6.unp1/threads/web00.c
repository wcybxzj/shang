#include "web.h"
#include "http.h"


struct file filearr[MAXFILES];

//专用于线程
void pthread_deal_http(struct file *ptr)
{
    int fd, len, sum, ret, output_fd;
    char buffer[MAXFILES];
    int content_length = 0;
    int data_read = 0;
    int read_index = 0;
    int checked_index = 0;
    int start_line = 0;
    enum CHECK_STATE checkstate = CHECK_STATE_LINE;

	fd = ptr->f_fd;
	output_fd = ptr->output_fd;

	//解析response line
    while (1) {
        bzero(buffer, sizeof(buffer));
        data_read = recv(fd, buffer+start_line, \
                sizeof(buffer)-start_line, 0);
        if (data_read < 0) {
			perror("recv");
			exit(1);
        }else if (data_read == 0){
			printf("EOF\n");
			exit(1);
        }

        read_index += data_read;
        enum HTTP_RESPONSE_CODE result = parse_response_content( buffer,\
				&checked_index, &checkstate, read_index, \
				&start_line, &content_length);

        if (result == NEED_RESPONSE_HEADER) {
            continue;
        }else if(result == GET_RESPONSE_HEADER){
            break;
        }
    }

    sum=0;
    len =  data_read-checked_index;
    if (len>0) {
        sum += len;
        ret = write(output_fd, buffer+checked_index, len);
		if (ret == -1) {
			perror("write");
			exit(1);
		}
    }

    while (1) {
        bzero(buffer, sizeof(buffer));
        len = recv(fd, buffer, sizeof(buffer), 0);
        if (len==0) {
            break;
        }else if (len<0){
			perror("recv");
			exit(1);
        }
        sum += len;
        write(output_fd, buffer, len);
        if (sum == content_length) {
            break;
        }
    }

	close(fd);
	close(output_fd);
}

void  do_get_read(void *vptr)
{
	int fd;
	struct file *fptr;
	char *file_name;
	struct timeval start, end;
	fptr = (struct file *) vptr;
	gettimeofday( &start, NULL );

	fd = Tcp_connect(fptr->f_host, fptr->f_port);
	fptr->f_fd = fd;
	write_get_cmd(fptr);

	//创建子请求的文件
	file_name = basename(fptr->f_name);
	remove(file_name);
	fptr->output_fd = open(file_name, \
			O_CREAT|O_APPEND|O_RDWR, 0644);
	if (fptr->output_fd == -1) {
		perror("open");
		exit(1);
	}
	pthread_deal_http(fptr);

	gettimeofday( &end, NULL );
	//printf("end.tv_usec:%lu\n", end.tv_usec);
	//printf("start.tv_usec:%lu\n",start.tv_usec);

	/*printf("f_name:%s, used-time:%g sec\n",\
			 fptr->f_name, \
			end.tv_sec-start.tv_sec+((end.tv_usec-start.tv_usec)/1000000.0));*/
}

//串行发访问index.html里的所有连接
void func0(int nfiles)
{
	int i;
	for (i = 0; i < nfiles ; i++) {
		 do_get_read(&filearr[i]);
	}
}


int main(int argc, char *argv[])
{
	int nfiles;
	char *host, *port ,*page;

	struct timeval start, end;
	gettimeofday( &start, NULL );
	if (argc != 4) {
		printf("./web 192.168.91.11 1234 /index.html\n");
		exit(1);
	}

	host = argv[1];
	port = argv[2];
	page = argv[3];

	home_page(host, port, page, filearr, &nfiles);

	//int i;
	//for (i = 0; i < nfiles; i++) {
	//	printf("%s\n", filearr[i].f_name);
	//	printf("%ld\n", strlen(filearr[i].f_name));
	//}
	//exit(1);

	func0(nfiles);

	gettimeofday( &end, NULL );
	printf("main, used-time:%g sec\n",\
			end.tv_sec-start.tv_sec+((end.tv_usec-start.tv_usec)/1000000.0));
	exit(0);
}
