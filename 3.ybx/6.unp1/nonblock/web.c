#include "web.h"

int main(int argc, char *argv[])
{
	int i, n, fd, ret, nfiles, flags, error;
	socklen_t error_size;
	int nlefttoread, nlefttoconn;
	struct file filearr[MAXFILES];
	int maxconn, current_conn;
	char *host, *port ,*page;
	char buf[MAXLINE];
	if (argc != 5) {
		printf("./web 3 192.168.91.11 1234 /index.html\n");
		exit(1);
	}

	maxconn = atoi(argv[1]);
	host = argv[2];
	port = argv[3];
	page = argv[4];

	home_page(host, port, page, filearr, &nfiles);

	//for (i = 0; i < nfiles; i++) {
	//	printf("%s\n", filearr[i].f_name);
	//	printf("%d\n", strlen(filearr[i].f_name));
	//}

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
				if (flags==F_INIT || flags == F_DONE || flags == F_FAIL) {
					continue;
				}
				printf("===============================================\n");
				printf("f_name:%s fd:%d flags:%d\n", \
						filearr[i].f_name,filearr[i].f_fd, flags);

				if (flags & F_CONNECTING &&  \
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
						free(filearr[i].f_name);
						close(fd);
						current_conn--;
						nlefttoread--;
					}else{
						printf("nonblock fd connect success for %s, fd:%d\n" ,\
								filearr[i].f_name, filearr[i].f_fd);
						FD_CLR(fd, &wset);
						write_get_cmd(&filearr[i]);
					}
				}else if(flags & F_READING && FD_ISSET(fd, &rset)){
					ret = read(fd, buf, sizeof(buf));
					printf("read ret:%d\n", ret);
					if (ret ==0) {
						printf("f_name:%s EOF\n", filearr[i].f_name);
						free(filearr[i].f_name);
						close(fd);
						filearr[i].f_flags = F_DONE;
						FD_CLR(fd, &rset);
						current_conn--;
						nlefttoread--;
					}else if(ret < 0){
						perror("read");
						exit(1);
					}else{
						printf("read!!! %d bytes from %s\n", ret, filearr[i].f_name);
						write(1, buf, ret);
					}
				}
			}
		}
	}
	return 0;
}
