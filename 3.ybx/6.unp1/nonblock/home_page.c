#include "web.h"

void home_page(char *host, char *port, const char *page, \
		struct file *filearr, int *nfiles){

	int sd, n, i, len;
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

	int j = 0;
    char *tmp = NULL;
	for (i = 1; 1 ; i++) {
        bzero(line, sizeof(line));
        tmp = fgets(line, sizeof(line), fd);
        if (tmp == NULL) {
            break;
        }
		if (i>3) {
			len = strlen(line);
			line[len-1]='\0';
			filearr[j].f_name = malloc(strlen(line));
			filearr[j].f_name = strndup(line, strlen(line));
			filearr[j].f_host = host;
			filearr[j].f_port = port;
			filearr[j].f_flags = F_INIT;
			j++;
		}
    }

	*nfiles = j;

	//printf("j:%d\n", j);



    close( sd );
    fclose(fd);
}

