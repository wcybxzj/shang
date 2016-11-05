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
			pthread_exit((void *) "recv");
        }else if (data_read == 0){
			pthread_exit(NULL);
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
			pthread_exit((void *) "write");
		}
    }

    while (1) {
        bzero(buffer, sizeof(buffer));
        len = recv(fd, buffer, sizeof(buffer), 0);
        if (len==0) {
            break;
        }else if (len<0){
			pthread_exit((void *) "recv");
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

void * do_get_read(void *vptr)
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
		pthread_exit((void*) "open");
	}
	pthread_deal_http(fptr);

	gettimeofday( &end, NULL );

	//printf("end.tv_usec:%lu\n", end.tv_usec);
	//printf("start.tv_usec:%lu\n",start.tv_usec);
	/*printf("tid:%d,f_name:%s, used-time:%g sec\n",\
			gettid(), fptr->f_name, \
			(end.tv_sec-start.tv_sec)+((end.tv_usec-start.tv_usec)/1000000.0));*/
	pthread_exit(NULL);
}
//测试方法:for i in {1..30}; do ./web04 192.168.91.11 80 /index.html; done
//运行30此取得平均时间

//串行发访问index.html里的所有连接,进行pthread_join 
void func1(int nfiles)
{
	pid_t pid;
	int i;
	for (i = 0; i < nfiles ; i++) {
		pid =  fork();
		if (pid==0) {
			do_get_read(&filearr[i]);
		} else if (pid>0) {
			//
		}else{
			perror("fork");
			exit(1);
		}
	}
	for (i = 0; i < nfiles; i++) {
		wait(NULL);
	}
}


int main(int argc, char *argv[])
{
	int nfiles;
	char *host, *port ,*page, *funcN;

	struct timeval start, end;
	gettimeofday( &start, NULL );


	if (argc != 5) {
		printf("./web func1 192.168.91.11 1234 /index.html\n");
		exit(1);
	}

	funcN = argv[1];
	host = argv[2];
	port = argv[3];
	page = argv[4];

	home_page(host, port, page, filearr, &nfiles);

	//int i;
	//for (i = 0; i < nfiles; i++) {
	//	printf("%s\n", filearr[i].f_name);
	//	printf("%ld\n", strlen(filearr[i].f_name));
	//}
	//exit(1);

	if (strcmp("func1", funcN)!=0) {
		printf("./web func1 192.168.91.11 1234 /index.html\n");
		exit(1);
	}

	func1(nfiles);

	gettimeofday( &end, NULL );
	printf("main tid:%d, used-time:%g sec\n",\
			gettid(), \
			(end.tv_sec-start.tv_sec)+((end.tv_usec-start.tv_usec)/1000000.0));
	pthread_exit(NULL);
}
