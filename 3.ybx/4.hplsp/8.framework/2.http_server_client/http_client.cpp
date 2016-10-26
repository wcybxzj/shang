#include "http.h"

//Host必须
#define GET_CMD "GET %s HTTP/1.1\r\nHost: 192.168.91.11:1234\r\n\r\n"

#define MAX 100
char *content_arr[MAX];

int main( int argc, char* argv[] )
{
	if( argc <= 3 )
	{
		printf( "usage: %s ip_address port_number index.html\n", \
				basename( argv[0] ) );
		return 1;
	}

	int i, ret, len, sd;
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	char *page= argv[3];
	struct sockaddr_in address;
	char buf[BUFFER_SIZE];

	//socket
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );
	sd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( sd >= 0 );

	int val=1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){ 
		perror("setsockopt()");
		exit(0);
	}   

	//bind 
	ret = connect( sd, ( struct sockaddr* )&address, sizeof( address ) );
	if (ret !=0) {
		perror("connect()");
		exit(1);
	}

	bzero(buf, sizeof(buf));
	snprintf(buf, sizeof(buf), GET_CMD, page);
	ret = send(sd, buf, strlen(buf), 0);
	if (ret == -1) {
		perror("send()");
		exit(1);
	}

	//失败
	//bzero(buf, BUFFER_SIZE);
	//while (1) {
	//	ret = read(sd, buf, BUFFER_SIZE);
	//	//ret = recv(sd, buf, BUFFER_SIZE, 0);
	//	if (ret<=0) {
	//		break;
	//	}
	//	write(1, buf, ret);
	//}

	//成功
	FILE *fd = fdopen(sd, "r");
	if (fd== NULL) {
		perror("fdopen");
		exit(1);
	}
	for (i = 0; i < 3; i++) {
		fgets(buf, sizeof(buf), fd);
		//printf("%s",buf);
	}
	bzero(buf, BUFFER_SIZE);
	while (1) { 
		len = fread(buf, 1, BUFFER_SIZE, fd); 
		if (len<=0) { 
			break; 
		} 
		fwrite(buf, 1, len, stdout); 
	}

	return 0;
}
