#include "http.h"

//杰作2016
//源代码有功能缺陷 http header host后无法解析，此代码更正
//并且完善功能成为一个可以任意查看文件的http server
//http://192.168.91.11:1234/index.html  200
//http://192.168.91.11:1234/index2.html 200
//http://192.168.91.11:1234/index3.html 404
int main( int argc, char* argv[] )
{
	if( argc <= 2 )
	{
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}

	int i, fd;
	int data_read = 0;
	int read_index = 0;
	int checked_index = 0;
	int start_line = 0;
	char buffer[ BUFFER_SIZE ];
	char *buffer_tmp;
	CHECK_STATE checkstate;
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	struct sockaddr_in address;

	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );
	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( listenfd >= 0 );

	int val=1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){ 
		perror("setsockopt()");
		exit(0);
	}   

	int ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
	assert( ret != -1 );
	ret = listen( listenfd, 5 );
	assert( ret != -1 );
	struct sockaddr_in client_address;

	int len;
	char *file_buf;
	int file_fd;
	bool valid;
	struct stat file_stat;
	char *file_name = (char *)malloc(50);
	char header_buf[BUFFER_SIZE];
	struct iovec iv[2];

recv:
	//printf("while 1\n");
	while (1) {
		socklen_t client_addrlength = sizeof( client_address );
		fd = accept( listenfd, ( struct sockaddr* )&client_address, \
				&client_addrlength );
		if( fd < 0 )
		{
			printf( "errno is: %d\n", errno );
		}
		else
		{
			memset( buffer, '\0', BUFFER_SIZE );
			data_read = 0;
			read_index = 0;
			checked_index = 0;
			start_line = 0;
			checkstate = CHECK_STATE_LINE;
			while( 1 )
			{
				//printf("block recv\n");
				data_read = recv( fd, buffer + read_index, \
						BUFFER_SIZE - read_index, 0 );
				//printf("unblock recv\n");
				valid = true;
				if ( data_read == -1 )
				{
					printf( "ERROR:reading failed\n" );
					goto recv;
				}
				else if ( data_read == 0 )
				{
					printf( "ERROR:remote client has closed the connection\n" );
					goto recv;
				}
				printf("========原始========\n");
				printf("%s\n",buffer);
				printf("========转义========\n");
				for (i = 0; i < data_read; i++) {
					if (buffer[i]=='\r') {
						printf("\\r");
					}else if (buffer[i]=='\t') {
						printf("\\t\n");
					}else if (buffer[i]=='\n') {
						printf("\\n\n");
					}else{
						printf("%c",buffer[i]);
					}
				}
				printf("--------------------\n");
				read_index += data_read;
				HTTP_CODE result = parse_content( buffer, checked_index, \
						checkstate, read_index, start_line ,&file_name);
				printf("---%s\n", file_name);
				printf("---result:%d\n", result);
				if( result == NO_REQUEST )
				{
					continue;
				}
				else if( result == GET_REQUEST )
				{
					chdir("www");
					if ( stat( file_name, &file_stat ) < 0 ) {
						valid = false;
					}else {
						if ( S_ISDIR( file_stat.st_mode ) ) {
							valid = false;
						}else if( file_stat.st_mode &S_IROTH ){
							file_fd = open( file_name, O_RDONLY );
							file_buf = new char[ file_stat.st_size + 1 ];
							memset( file_buf, '\0', file_stat.st_size +1 );
							if ( read( file_fd, file_buf, file_stat.st_size ) < 0 ) {
								valid = false;
							}
							close(file_fd);
						}
						else {
							valid = false;
						}
					}

					//send( fd, szret[0], strlen( szret[0] ), 0 );
					break;
				}
				else
				{
					valid = false;
					//send( fd, szret[1], strlen( szret[1] ), 0 );
					break;
				}
			}
			//printf(">>>>>valid:%d<<<<<\n", valid);
			len = 0;
			memset( header_buf, '\0', BUFFER_SIZE );
			if ( valid ) {
				ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s \r\n", \
						"HTTP/1.1", status_line[0] );
				len +=ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, \
						"Content-Length: %lld\r\n", file_stat.st_size );
				len +=ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "%s", \
						"\r\n" );
				iv[ 0 ].iov_base = header_buf;
				iv[ 0 ].iov_len = strlen( header_buf );
				iv[ 1 ].iov_base = file_buf;
				iv[ 1 ].iov_len = file_stat.st_size;
				ret = writev( fd, iv, 2);
				delete [] file_buf;
			} else {
				ret = snprintf( header_buf, BUFFER_SIZE-1, \
						"%s %s\r\n", "HTTP/1.1", status_line[1] );
				len += ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, \
						"Content-Length: %lld\r\n", strlen(status_line[1]));
				len +=ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, \
						"%s", "\r\n");
				len +=ret;
				ret = snprintf( header_buf +len , BUFFER_SIZE-1-len,\
						"%s", "404 Not Found\r\n");
				//printf("%s\n", header_buf);
				send( fd, header_buf, strlen( header_buf), 0);
			}
		}
		close( fd );
	}
	free(file_name);
	close( listenfd );
	return 0;
}
