#include "unp.h"

#define MAXFILES 20

//tcp state
enum F_FLAGS{
	F_INIT		 = 0,
	F_CONNECTING = 1,
	F_READING	 = 2,
	F_DONE		 = 4,
	F_FAIL		 = 8
};

//http parse state
enum HTTP_PARSE_FLAGS{
	HTTP_INIT	= 0,
	HTTP_HEADER = 1,
	HTTP_BODY	= 2
};

struct file{
	char *f_name;
	char *f_host;
	char *f_port;
	int f_fd;//socket fd
	FILE *f_fp;//socket fp
	int output_fd;
	int content_length;//http header中content_length
	int current_content_length;
	enum HTTP_PARSE_FLAGS http_flags;
	enum F_FLAGS f_flags;
};

#define GET_CMD "GET %s HTTP/1.1\r\nHost: 192.168.91.11:1234\r\n\r\n"

fd_set rset, wset;
int maxfd;

//0 success
//-1 failed
void home_page(char *host, char *port, const char *page, \
		struct file *filearr, int *nfiles);

void start_connect(struct file* fptr);
void write_get_cmd(struct file* fptr);
