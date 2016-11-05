#include "web.h"
#include "http.h"

//如果放在函数中stack size不够会段错误
struct file filearr[MAXFILES];

int setblocking(int sd){
	int old = fcntl(sd, F_GETFL);
	fcntl(sd, F_SETFL, old & ~O_NONBLOCK);
	return old;
}

//unp版本存在缺陷如果请求的不是本机的server丢任何一个子请求在网络中丢包
//select都会阻塞
//./web 7 192.168.1.110 80 /index.html 请求地址是其他机器,并发connect7个结果select直接block
void func1(int maxconn, int nfiles)
{
	enum F_FLAGS flags;
	int i, n, fd, ret, error;
	socklen_t error_size;
	int nlefttoread, nlefttoconn;
	int current_conn;
	struct timeval start, end;
	gettimeofday( &start, NULL );
	nlefttoread = nlefttoconn = nfiles;
	//printf("total nlefttoread:%d\n", nlefttoread);
	current_conn = 0;
	while (nlefttoread > 0) {
		while (current_conn < maxconn && nlefttoconn > 0) {
			//find this i
			for (i = 0; i < nfiles ; i++) {
				if (filearr[i].f_flags==0) {
					break;
				}
			}
			if (i == nfiles) {
				printf("all filearr is loaded\n");
				break;
			}
			start_connect(&filearr[i]);
			current_conn++;
			nlefttoconn--;
		}

		//for (i = 0; i < nfiles; i++) {
		//	printf("%d\n",filearr[i].f_flags);
		//}
		//exit(1);

		//printf("nlefttoconn:%d\n",nlefttoconn);
		//printf("nlefttoread:%d\n",nlefttoread);
		//printf("block\n");
		n = select(maxfd+1, &rset , &wset, NULL, NULL);
		//printf("unblock n:%d\n",n);

		if (n==0) {
			printf("select timeout\n");
		}else if(n<0){
			perror("select");
			exit(1);
		}else{
			for (i = 0; i < nfiles; i++) {
				flags = filearr[i].f_flags;
				fd = filearr[i].f_fd;
				if (flags==F_INIT || flags == F_DONE || flags == F_FAIL) {
					continue;
				}
				if (flags ==  F_CONNECTING &&  \
						(FD_ISSET(fd, &rset)|| FD_ISSET(fd, &wset))) {
					error_size = sizeof(error);
					ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &error_size);
					if (ret < 0 || error != 0) {
						printf("%s\n",strerror(error));
						exit(1);
						FD_CLR(fd, &rset);
						FD_CLR(fd, &wset);
						filearr[i].f_flags = F_FAIL;
						close(fd);
						current_conn--;
						nlefttoread--;
					}else{
						FD_CLR(fd, &wset);
						FD_CLR(fd, &rset);
						//write_get_cmd(&filearr[i]);
						close(fd);
						filearr[i].f_flags = F_DONE;
						current_conn--;
						nlefttoread--;
						//printf("f_name:%s, nlefttoread:%d\n", \
								filearr[i].f_name, nlefttoread);
					}
				}
			}
		}
	}
	gettimeofday( &end, NULL );
	printf("main, used-time:%g sec\n",\
			end.tv_sec-start.tv_sec+((end.tv_usec-start.tv_usec)/1000000.0));
	for (i = 0; i < nfiles; i++) {
		free(filearr[i].f_name);
	}
}

//for i in {1..10}; do ./web 2 127.0.0.1 80 /index.html; done
int main(int argc, char *argv[])
{
	int i, nfiles;
	int maxconn;
	char *host, *port ,*page;
	if (argc != 5) {
		printf("./web 3 192.168.91.11 80 /index.html\n");
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

	func1(maxconn, nfiles);

	return 0;
}
