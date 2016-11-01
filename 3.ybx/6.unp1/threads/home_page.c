#include "web.h"

//解析home_page从中获取要并发connect的请求
void home_page(char *host, char *port, const char *page, \
		struct file *filearr, int *nfiles){

	int sd, n, j , len, num;
	char line[MAXLINE];
	sd = tcp_connect(host, port);

	n = snprintf(line, MAXLINE, GET_CMD, page);
	//printf("%s\n", line);
	Writen(sd, line, n);

    FILE* fd = fdopen(sd, "r");
    if (fd == NULL) {
        perror("fdopen");
        exit(1);
    }

    int content_length;
    while (1) {
        fgets(line, sizeof(line), fd);
        if (strcmp(line, "\r\n")==0) {
            break;
        }
        //Content-Length: 5928
        if (strncmp(line, "Content-Length", strlen("Content-Length")) == 0) {
            sscanf(line, "Content-Length: %d", &content_length);
            //printf("%s", line);
        }
    }
	//printf("%d\n", content_length);

	j = 0;
    num = 0;
    while (1) {
        if(fgets(line, sizeof(line), fd) == NULL) {
            break;
        }else{
            num+=strlen(line);
			len = strlen(line);
			line[len-1]='\0';
			filearr[j].f_name = malloc(strlen(line));
			filearr[j].f_name = strndup(line, strlen(line));
			filearr[j].f_host = host;
			filearr[j].f_port = port;

			filearr[j].content_length = 0;
			filearr[j].current_content_length = 0;

			filearr[j].f_flags = F_INIT;

			j++;
        }
        //客户端读取到足够的content-length主动关闭
        //对付开启keep-alive的web-server
        if (num == content_length) {
            break;
        }
    }

	*nfiles = j;

	//printf("j:%d\n", j);

    close( sd );
    fclose(fd);
}

