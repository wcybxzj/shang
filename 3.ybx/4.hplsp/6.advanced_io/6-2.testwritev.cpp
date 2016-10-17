#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>
#define BUFFER_SIZE 1024
//static const char* status_line[2] = { "200 OK", "500 Internal server error" };
static const char* status_line[2] = { "200 OK", "404 Not Found" };

bool daemonize(){
	pid_t pid = fork();
	if ( pid < 0 ) {
		return false;
	}else if( pid > 0 ) {
		exit( 0 );
	}
	umask( 0 );
	pid_t sid = setsid();
	if ( sid < 0 ) {
		return false;
	}

	printf( "%d\n", getpid());

	//if ( ( chdir( "/" ) ) < 0 ) {
	//	return false;
	//}

	close( STDIN_FILENO );
	close( STDOUT_FILENO );
	close( STDERR_FILENO );
	open( "/dev/null", O_RDONLY );
	open( "/dev/null", O_RDWR );
	open( "/dev/null", O_RDWR );
	return false;
}

//修正版还加了守护进程
// ./6-2.testwritev 192.168.91.11 1234 333.txt  返回200
// ./6-2.testwritev 192.168.91.11 1234 123.txt	返回404
int main(int argc, char *argv[])
{
	if ( argc <= 3 ) {
		printf("useage: %s ip port filename\n", basename( argv[0] ) );
		return 1;
	}

	//daemonize();

	int fd;
	int connfd;
	int len;
	socklen_t  client_addrlength;
	char header_buf[ BUFFER_SIZE ];
	struct sockaddr_in client;
	char* file_buf;
	struct stat file_stat;
	struct iovec iv[2];
	bool valid;
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	const char* file_name = argv[3];
	struct sockaddr_in address;

	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr);
	address.sin_port = htons( port );

	int sock = socket( PF_INET, SOCK_STREAM, 0 );
	assert( sock >= 0);
	int ret = bind( sock, (struct sockaddr* )&address, sizeof( address ) );
	assert( ret != -1);
	ret = listen( sock, 5);
	assert( ret != -1);

	while (1) {
		client_addrlength = sizeof( client );
		connfd = accept( sock, ( struct sockaddr* )&client, &client_addrlength );
		if ( connfd < 0 ) {
			printf("errno is: %d\n", errno );
		}else {
			memset( header_buf, '\0', BUFFER_SIZE );
			valid = true;
			len = 0;
			if ( stat( file_name, &file_stat ) < 0 ) {
				valid = false;
			}else {
				if ( S_ISDIR( file_stat.st_mode ) ) {
					valid = false;
				}else if( file_stat.st_mode &S_IROTH ){
					fd = open( file_name, O_RDONLY );
					file_buf = new char[ file_stat.st_size + 1 ];
					memset( file_buf, '\0', file_stat.st_size +1 );
					if ( read( fd, file_buf, file_stat.st_size ) < 0 ) {
						valid = false;
					}
				}
				else {
					valid = false;
				}
			}
			if ( valid ) {
				ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s \r\n", "HTTP/1.1", status_line[0] );
				len +=ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "Content-Length: %lld\r\n", file_stat.st_size );
				len +=ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "%s", "\r\n" );
				iv[ 0 ].iov_base = header_buf;
				iv[ 0 ].iov_len = strlen( header_buf );
				iv[ 1 ].iov_base = file_buf;
				iv[ 1 ].iov_len = file_stat.st_size;
				ret = writev( connfd, iv, 2);
				delete [] file_buf;
			} else {
				ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", status_line[1] );
				len += ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "Content-Length: %lld\r\n", strlen(status_line[1]));
				len +=ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "%s", "\r\n");
				len +=ret;
				ret = snprintf( header_buf +len , BUFFER_SIZE-1-len, "%s", "404 Not Found\r\n");
				printf("%s\n", header_buf);
				send( connfd, header_buf, strlen( header_buf), 0);
			}
			close( connfd );
		}
	}

	close( sock );
	return 0;
}
