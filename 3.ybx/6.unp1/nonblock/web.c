#include "web.h"


int main(int argc, char *argv[])
{
	int nfiles, struct file filearr[MAXFILES];
	int maxconn, nconn;
	char *host, *page;
	int port;
	if (argc != 5) {
		printf("./a.out conn host port index.html\n");
		exit(1);
	}

	maxconn = atoi(argv[1]);
	host = argv[2];
	port = atoi(argv[3]);
	page = argv[3];

	home_page(host, port, page, filearr, &nfiles);


	return 0;
}
