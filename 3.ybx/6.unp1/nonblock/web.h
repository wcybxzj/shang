#include "unp.h"

#define MAXFILES 20

enum F_FLAGS{
	F_INIT,
	F_CONNECTING,
	F_READING,
	F_DONE
};

struct file{
	char *f_name;
	char *f_host;
	int f_fd;
	enum F_FLAGS f_flags;
};

#define GET_CMD "GET %s HTTP/1.0 \r\n\r\n" 


fd set rset, wset;


//0 success
//-1 failed
void home_page(const char *host,\
				const int *port,\
				const char *page);

void start_connect(struct file* fptr);
void write_get_cmd(struct file* fptr);
