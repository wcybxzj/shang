#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0,
				   CHECK_STATE_HEADER,
				   CHECK_STATE_CONTENT };

enum LINE_STATUS { LINE_OK = 0,
					LINE_BAD,
					LINE_OPEN };

enum HTTP_CODE { NO_REQUEST,
	GET_REQUEST,
	BAD_REQUEST,
	FORBIDDEN_REQUEST,
	INTERNAL_ERROR,
	CLOSED_CONNECTION };

static const char* status_line[2] = { "200 OK", "404 Not Found" };
static const char* szret[] = { "I get a correct result\n", "Something wrong\n" };

void deal_url(char *url , char **name){
	*name = strdup("index.html");
	if (strcmp(url, "/") != 0) {
		url++;
		*name = url;
	}
}

LINE_STATUS parse_line( char* buffer, int& checked_index, int& read_index )
{
	char temp;
	for ( ; checked_index < read_index; ++checked_index )
	{
		temp = buffer[ checked_index ];
		if ( temp == '\r' )
		{
			if ( ( checked_index + 1 ) == read_index )
			{
				return LINE_OPEN;
			}
			else if ( buffer[ checked_index + 1 ] == '\n' )
			{
				buffer[ checked_index++ ] = '\0';
				buffer[ checked_index++ ] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if( temp == '\n' )
		{
			if( ( checked_index > 1 ) &&  buffer[ checked_index - 1 ] == '\r' )
			{
				buffer[ checked_index-1 ] = '\0';
				buffer[ checked_index++ ] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
	}
	return LINE_OPEN;
}

HTTP_CODE parse_requestline( char* szTemp, CHECK_STATE& checkstate, char** name)
{
	char* szURL = strpbrk( szTemp, " \t" );
	//printf("szTemp:%s\n", szTemp);
	if ( ! szURL )
	{
		return BAD_REQUEST;
	}
	*szURL++ = '\0';

	char* szMethod = szTemp;
	if ( strcasecmp( szMethod, "GET" ) == 0 )
	{
		printf( "The request method is GET\n" );
	}
	else
	{
		return BAD_REQUEST;
	}

	szURL += strspn( szURL, " \t" );
	char* szVersion = strpbrk( szURL, " \t" );
	if ( ! szVersion )
	{
		return BAD_REQUEST;
	}

	*szVersion++ = '\0';
	szVersion += strspn( szVersion, " \t" );
	if ( strcasecmp( szVersion, "HTTP/1.1" ) != 0 )
	{
		return BAD_REQUEST;
	}
	if ( strncasecmp( szURL, "http://", 7 ) == 0 )
	{
		szURL += 7;
		szURL = strchr( szURL, '/' );
	}
	if ( ! szURL || szURL[ 0 ] != '/' )
	{
		return BAD_REQUEST;
	}
	//URLDecode( szURL );
	printf( "The request URL is: %s\n", szURL );

	deal_url(szURL, name);

	checkstate = CHECK_STATE_HEADER;
	return NO_REQUEST;
}

//GET / HTTP/1.1\r\n
//Host: 192.168.91.11:1234\r\n
//Connection: keep-alive\r\n
//Cache-Control: max-age=0\r\n
//Upgrade-Insecure-Requests: 1\r\n
//User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36\r\n
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n
//Accept-Encoding: gzip, deflate, sdch\r\n
//Accept-Language: zh-CN,zh;q=0.8,en;q=0.6,it;q=0.4,fil;q=0.2,zh-TW;q=0.2\r\n
HTTP_CODE parse_headers( char* szTemp )
{
	int all;
	char *tmp;
	if ( szTemp[ 0 ] == '\0' )
	{
		return GET_REQUEST;
	}
	else
	{
		all = strlen(szTemp);
		tmp = strpbrk(szTemp, ":");
		if(tmp){
			*tmp++ = '\0';
			*tmp++ = '\0';
			printf("%s:%s\n", szTemp,tmp);
		}
		else
		{
			printf( "I can not handle this header\n" );
		}
	}
	return NO_REQUEST;
}

HTTP_CODE parse_content( char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line, char **ret_name )
{
	LINE_STATUS linestatus = LINE_OK;
	HTTP_CODE retcode = NO_REQUEST;
	char *name = (char *)malloc(50);

	while( ( linestatus = parse_line( buffer, checked_index, read_index ) ) == LINE_OK )
	{
		printf("--OK--:");
		char* szTemp = buffer + start_line;
		start_line = checked_index;
		switch ( checkstate )
		{
			case CHECK_STATE_REQUESTLINE:
			{
				bzero(name, strlen(name));
				retcode = parse_requestline( szTemp, checkstate, &name);
				if ( retcode == BAD_REQUEST )
				{
					return BAD_REQUEST;
				}
				printf(">>>%s\n", name);
				*ret_name = strdup(name);
				break;
			}
			case CHECK_STATE_HEADER:
			{
				retcode = parse_headers( szTemp );
				if ( retcode == BAD_REQUEST )
				{
					return BAD_REQUEST;
				}
				else if ( retcode == GET_REQUEST )
				{
					return GET_REQUEST;
				}
				break;
			}
			default:
			{
				return INTERNAL_ERROR;
			}
		}
	}
	if( linestatus == LINE_OPEN )
	{
		return NO_REQUEST;
	}
	else
	{
		return BAD_REQUEST;
	}
	free(name);
}

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
			checkstate = CHECK_STATE_REQUESTLINE;
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
