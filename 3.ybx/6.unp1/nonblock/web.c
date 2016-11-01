#include "web.h"
#include "http.h"

int setblocking(int sd){
	int old = fcntl(sd, F_GETFL);
	fcntl(sd, F_SETFL, old & ~O_NONBLOCK);
	return old;
}

//非阻塞没能实现 每个子请求去并发下载,改用多线程实现6.unp1/threads/web03.c
void deal_reading(int fd, struct file filearr[], \
					int *current_conn, int *nlefttoread)
{
	if (fgets(buf, sizeof(buf), filearr[i].f_fp) == NULL) {
		if (errno == EINPROGRESS) {
			continue;
		}else{
			perror("fgets(!)");
			exit(1);
		}
	}else{
		filearr[i].current_content_length+=strlen(buf);
		write(filearr[i].output_fd, buf, strlen(buf));
	}

	if (filearr[i].current_content_length == content_length) {
		printf("f_name:%s EOF\n", filearr[i].f_name);
		close(fd);
		filearr[i].f_flags = F_DONE;
		FD_CLR(fd, &rset);
		current_conn--;
		nlefttoread--;
		close(filearr[i].output_fd);
	}
}

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
	enum F_FLAGS flags;
	int i, n, fd, ret, nfiles, error, content_length;
	socklen_t error_size;
	int nlefttoread, nlefttoconn;
	struct file filearr[MAXFILES];
	int maxconn, current_conn;
	char *host, *port ,*page, *file_name;

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

		printf("block select\n");
		n = select(maxfd+1, &rset , &wset, NULL, NULL);
		printf("unblcok select n:%d\n", n);
		if (n==0) {
			printf("select timeout\n");
		}else if(n<0){
			perror("select");
			exit(1);
		}else{
			for (i = 0; i < nfiles; i++) {
				flags = filearr[i].f_flags;
				fd = filearr[i].f_fd;
				content_length = filearr[i].content_length;

				if (flags==F_INIT || flags == F_DONE || flags == F_FAIL) {
					continue;
				}

				printf("===============================================\n");
				printf("f_name:%s fd:%d flags:%d\n",\
						filearr[i].f_name, filearr[i].f_fd, flags);

				if (flags ==  F_CONNECTING &&  \
						(FD_ISSET(fd, &rset)|| FD_ISSET(fd, &wset))) {
					printf("F_CONNECTING f_name:%s\n", filearr[i].f_name);
					error_size = sizeof(error);
					ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &error_size);
					if (ret < 0 || error != 0) {
						printf("nonblocking connect fail for %s\n", \
								filearr[i].f_name);
						FD_CLR(fd, &rset);
						FD_CLR(fd, &wset);
						filearr[i].f_flags = F_FAIL;
						//free(filearr[i].f_name);
						close(fd);
						current_conn--;
						nlefttoread--;
					}else{
						printf("nonblock fd connect success for %s, fd:%d\n" ,\
								filearr[i].f_name, filearr[i].f_fd);
						FD_CLR(fd, &wset);
						write_get_cmd(&filearr[i]);

						printf(">>write_get_cmd after: %d<<\n", filearr[i].f_flags);

						//创建子请求的文件
						file_name = basename(filearr[i].f_name);
						remove(file_name);
						filearr[i].output_fd = open(file_name, \
								O_CREAT|O_APPEND|O_RDWR,\
								0644);
						if (filearr[i].output_fd == -1) {
							perror("open");
							exit(1);
						}
					}
				}else if(flags == F_READING && FD_ISSET(fd, &rset)){
					//deal_reading(fd, filearr, &current_conn, &nlefttoread);
				}
			}
		}
	}

	for (i = 0; i < nfiles; i++) {
		free(filearr[i].f_name);
	}

	return 0;
}
