#include "http.h"

//Host必须
#define GET_CMD "GET %s HTTP/1.1\r\nHost: 192.168.91.11:1234\r\n\r\n"

#define MAX 100
char *content_arr[MAX];

//只能处理 keep-alive:close
void func1(int sd)
{
	int  len;
	int content_length;
	char buf[BUFFER_SIZE];
	int num = 0;
	//成功
	FILE *fd = fdopen(sd, "r");
	if (fd== NULL) {
		perror("fdopen");
		exit(1);
	}

	while (1) {
		bzero(buf, BUFFER_SIZE);
		fgets(buf, sizeof(buf)-1, fd);
		if (strcmp(buf, "\r\n")==0) {
			break;
		}
		//Content-Length: 5928
		if (strncmp(buf, "Content-Length", strlen("Content-Length")) == 0) {
			sscanf(buf, "Content-Length: %d", &content_length);
		}
	}

	num =0;
	while (1) {
		bzero(buf, BUFFER_SIZE);
		num = fread(buf, 1, sizeof(buf)-1, fd);
		if (num<=0) {
			break;
		}   
		fwrite(buf, 1, num, stdout);
		fflush(NULL);
	}   

	fclose(fd);
}

//处理 开启或关闭keep-alive
void func2(int sd)
{
	char buffer[BUFFER_SIZE];
	int content_length = 0;
	int data_read = 0;
	int read_index = 0;
	int checked_index = 0;
	int start_line = 0;
	CHECK_STATE checkstate = CHECK_STATE_LINE;

	while (1) {
		bzero(buffer, sizeof(buffer));
		data_read = recv(sd, buffer,sizeof(buffer),0);
		if (data_read < 0) {
			perror("recv");
			break;
		}
		//write(1, buffer, data_read);

		read_index += data_read;
		HTTP_RESPONSE_CODE result = parse_response_content( buffer, checked_index, \
				checkstate, read_index, start_line, &content_length);

	}
}


int main( int argc, char* argv[] )
{
	if( argc <= 3 )
	{
		printf( "usage: %s ip_address port_number /index.html\n", \
				basename( argv[0] ) );
		return 1;
	}

	int  ret, len, sd;
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

	//func1(sd);
	func2(sd);

	close(sd);
	return 0;
}
