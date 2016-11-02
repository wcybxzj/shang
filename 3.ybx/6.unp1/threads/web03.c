#include "web.h"
#include "http.h"

struct file filearr[MAXFILES];

//volatile  sig_atomic_t ndone;
//volatile  int ndone;
int ndone;
pthread_mutex_t ndone_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ndone_cond = PTHREAD_COND_INITIALIZER;

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

	if ( (ret = pthread_mutex_lock(&ndone_mutex)) != 0){
		pthread_exit((void *) "pthread_mutex_lock");
	}
	pthread_cond_signal(&ndone_cond);
	ptr->f_flags = F_DONE;
	ndone++;

	if ( (ret = pthread_mutex_unlock(&ndone_mutex)) != 0){
		pthread_exit((void *) "pthread_mutex_unlock");
	}
}

void * do_get_read(void *vptr)
{
	int fd;
	struct file *fptr;
	char *file_name;
	fptr = (struct file *) vptr;

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

	return(fptr);
}


//并发1 和并发20几乎没区别
//[root@web11 threads]# for i in {1..10}; do time ./web03 1 192.168.91.11 80 /index.html ; done 
//[root@web11 threads]# for i in {1..10}; do time ./web03 20 192.168.91.11 80 /index.html ; done 

//server 可以用自己的webserver或者nginx/apache
//server:
//cd 4.hplsp/8.framework/2.http_server_client
//./http_server 192.168.91.11 1234
//
//client:
//./web 3 192.168.91.11 1234 /index.html
//./web 3 192.168.91.11 1234 /index2.html
int main(int argc, char *argv[])
{
	pthread_t tid;
	int i, ret, nfiles, n;
	int nlefttoread, nlefttoconn;
	int maxconn, current_conn;
	char *host, *port ,*page;
	struct file *fptr;
	if (argc != 5) {
		printf("./web 3 192.168.91.11 1234 /index.html\n");
		exit(1);
	}

	maxconn = atoi(argv[1]);//并发数
	host = argv[2];
	port = argv[3];
	page = argv[4];

	home_page(host, port, page, filearr, &nfiles);

	//for (i = 0; i < nfiles; i++) {
	//	printf("%s\n", filearr[i].f_name);
	//	printf("%d\n", strlen(filearr[i].f_name));
	//}
	//exit(1);

	nlefttoread = nlefttoconn = nfiles;
	current_conn = 0;
	while (nlefttoread > 0) {
		while (current_conn < maxconn && nlefttoconn > 0) {
			//find this i
			for (i = 0; i < nfiles ; i++) {
				if (filearr[i].f_flags==F_INIT) {
					break;
				}
			}
			if (i == nfiles) {
				//printf("all filearr is loaded\n");
				break;
			}

			filearr[i].f_flags = F_CONNECTING;
			ret = pthread_create(&tid, NULL, do_get_read, &filearr[i]);
			if (ret!=0) {
				perror("pthread_crate");
				exit(1);
			}

			filearr[i].f_tid = tid;
			current_conn++;
			nlefttoconn--;
		}


        if ( (n = pthread_mutex_lock(&ndone_mutex)) != 0)
            errno = n, err_sys("pthread_mutex_lock error");

		//main thread 在unblock mutex的部分需要出让cpu调度
		//好让其他线程有就会lock mutex ndone++,并进入F_DONE,
		//好让 main thread,pthread_join,并开始创建新的工作进程
		//
		//办法1:sleep 太耗时
		//办法2:sched_yield 一般
		//办法3:条件变量 看web03

		while (ndone==0) {
			pthread_cond_wait(&ndone_cond, &ndone_mutex);	
		}

        if (ndone > 0) {
            for (i = 0; i < nfiles; i++) {
                if (filearr[i].f_flags == F_DONE) {
                    if ( (n = pthread_join(filearr[i].f_tid, (void **) &fptr)) != 0)
                        errno = n, err_sys("pthread_join error");

                    if (&filearr[i] != fptr){
						exit(1);
					}

                    fptr->f_flags = F_JOINED;
                    ndone--;
                    current_conn--;
                    nlefttoread--;
                }
            }
        }
        if ( (n = pthread_mutex_unlock(&ndone_mutex)) != 0)
            errno = n, err_sys("pthread_mutex_unlock error");
	}

	for (i = 0; i < nfiles; i++) {
		free(filearr[i].f_name);
	}

	return 0;
}
