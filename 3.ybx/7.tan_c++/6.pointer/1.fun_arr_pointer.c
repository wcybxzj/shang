#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX 3

struct client_data{
    int fd;
    struct sockaddr_in address;
    char *write_buf;
    char buf[BUFFER_SIZE];
};


int main(int argc, const char *argv[])
{
	int i;
	struct clinet_data arr[MAX];
	for (i = 0; i < MAX; i++) {
		arr[i].fd=-1;
	}

	//arr[1].fd =100;
	//arr[0].fd =arr[1];
	//for (i = 0; i <= MAX; i++) {
	//	printf("%d\n",arr[i]);
	//}

	return 0;
}
