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

	//int i;
	//for (i = 0; i <100 ; i++) {
	//	printf("tid:%d sleep i:%d\n", gettid(), i);
	//	sleep(1);
	//}

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
	/*
	printf("tid:%d,f_name:%s, used-time:%lu.%lu sec\n",\
			gettid(), fptr->f_name, \
			end.tv_sec-start.tv_sec, end.tv_usec-start.tv_usec);
	*/
	pthread_exit(NULL);
}



//测试1:效率第1
//串行发访问index.html里的所有连接,进行pthread_join 
//for i in {1..10}; do ./web04 192.168.91.11 80 /index.html; done
//main tid:88008, used-time:0.104561 sec
//main tid:88049, used-time:0.106454 sec
//main tid:88090, used-time:0.86397 sec
//main tid:88131, used-time:0.88457 sec
//main tid:88172, used-time:0.111802 sec
//main tid:88213, used-time:0.95553 sec
//main tid:88254, used-time:1.18446744073708645709 sec
//main tid:88295, used-time:0.97549 sec
//main tid:88336, used-time:0.92586 sec
//main tid:88377, used-time:0.121337 sec
void func1(int nfiles)
{
	pthread_t tid;
	int i, ret;
	for (i = 0; i < nfiles ; i++) {
		ret = pthread_create(&tid, NULL, do_get_read, &filearr[i]);
		if (ret!=0) {
			perror("pthread_crate");
			exit(1);
		}
		filearr[i].f_tid = tid;
		pthread_join(filearr[i].f_tid, NULL);
	}
}

//测试2:效率排第2
//并发访问index.html里的所有连接,不进行pthread_join 
//for i in {1..10}; do ./web04 192.168.91.11 80 /index.html; done
//main tid:88435, used-time:1.18446744073708655983 sec
//main tid:88476, used-time:0.33962 sec
//main tid:88517, used-time:0.37376 sec
//main tid:88558, used-time:0.35651 sec
//main tid:88599, used-time:0.24906 sec
//main tid:88640, used-time:0.37695 sec
//main tid:88681, used-time:0.44153 sec
//main tid:88722, used-time:0.37432 sec
//main tid:88763, used-time:0.31087 sec
//main tid:88804, used-time:0.23814 sec

void func2(int nfiles)
{
	pthread_t tid;
	int i, ret;
	for (i = 0; i < nfiles ; i++) {
		ret = pthread_create(&tid, NULL, do_get_read, &filearr[i]);
		if (ret!=0) {
			perror("pthread_crate");
			exit(1);
		}
	}
}

//测试3:效率第3
//并发访问index.html里的所有连接,进行pthread_join 
void func3(int nfiles)
{
	pthread_t tid;
	int i, ret;
	for (i = 0; i < nfiles ; i++) {
		ret = pthread_create(&tid, NULL, do_get_read, &filearr[i]);
		if (ret!=0) {
			perror("pthread_crate");
			exit(1);
		}
		filearr[i].f_tid = tid;
	}
	for (i = 0; i < nfiles ; i++) {
		pthread_join(filearr[i].f_tid, NULL);
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

	func1(nfiles);
	//func2(nfiles);
	//func3(nfiles);

	gettimeofday( &end, NULL );
	printf("main tid:%d, used-time:%lu.%lu sec\n",\
			gettid(), end.tv_sec-start.tv_sec, end.tv_usec-start.tv_usec);
	pthread_exit(NULL);
}
