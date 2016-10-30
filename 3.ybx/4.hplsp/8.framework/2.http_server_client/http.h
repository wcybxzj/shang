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

enum CHECK_STATE { CHECK_STATE_LINE = 0,//request line or response line
				   CHECK_STATE_HEADER,
				   CHECK_STATE_CONTENT };

enum LINE_STATUS { LINE_OK = 0,
					LINE_BAD,
					LINE_OPEN };


enum HTTP_RESPONSE_CODE {NEED_RESPONSE_HEADER,//仍需头数据
	GET_RESPONSE_HEADER,//头部处理完成
	BAD_RESPONSE_HEADER,//错误头部数据
	FORBIDDEN_RESPONSE,
	INTERNAL_RESPONSE_ERROR,
	CLOSED_RESPONSE_CONNECTION };


enum HTTP_REQUEST_CODE {NEED_RESQUEST_HEADER,//仍需头数据
	GET_REQUEST_HEADER,//头部处理完成
	BAD_REQUEST_HEADER,//错误头部数据
	FORBIDDEN_REQUEST,
	INTERNAL_REQUEST_ERROR,
	CLOSED_REQUEST_CONNECTION };

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

//GET /3.jpg HTTP/1.1
HTTP_REQUEST_CODE parse_requestline( char* szTemp, CHECK_STATE& checkstate, char** name)
{
	char* szURL = strpbrk( szTemp, " \t" );
	printf("szTemp:%s\n", szTemp);
	if ( ! szURL )
	{
		return BAD_REQUEST_HEADER;
	}
	*szURL++ = '\0';

	char* szMethod = szTemp;
	if ( strcasecmp( szMethod, "GET" ) == 0 )
	{
		printf( "The request method is GET\n" );
	}
	else
	{
		return BAD_REQUEST_HEADER;
	}

	szURL += strspn( szURL, " \t" );
	char* szVersion = strpbrk( szURL, " \t" );
	if ( ! szVersion )
	{
		return BAD_REQUEST_HEADER;
	}

	*szVersion++ = '\0';
	szVersion += strspn( szVersion, " \t" );
	if ( strcasecmp( szVersion, "HTTP/1.1" ) != 0 )
	{
		return BAD_REQUEST_HEADER;
	}
	if ( strncasecmp( szURL, "http://", 7 ) == 0 )
	{
		szURL += 7;
		szURL = strchr( szURL, '/' );
	}
	if ( ! szURL || szURL[ 0 ] != '/' )
	{
		return BAD_REQUEST_HEADER;
	}
	//URLDecode( szURL );
	printf( "The request URL is: %s\n", szURL );

	deal_url(szURL, name);

	checkstate = CHECK_STATE_HEADER;
	return NEED_RESQUEST_HEADER;
}

//HTTP/1.1 200 OK
HTTP_RESPONSE_CODE parse_responseline( char* szTemp, CHECK_STATE& checkstate)
{
	char* szURL = strpbrk( szTemp, " \t" );
	printf( "The request URL is: %s\n", szTemp );
	if ( ! szURL )
	{
		return BAD_RESPONSE_HEADER;
	}
	*szURL++ = '\0';//" 200 OK"

	char* http = szTemp;//"HTTP/1.1"
	if ( strcasecmp( http, "HTTP/1.1" ) == 0 )
	{
		//printf( "The request method is GET\n" );
	}
	else
	{
		return BAD_RESPONSE_HEADER;
	}
	int http_code;
	char http_str[BUFFER_SIZE];
	szURL += strspn( szURL, " \t" );//"200 OK"
	sscanf(szURL, "%d %s", &http_code, http_str);
	if (http_code != 200) {
		return BAD_RESPONSE_HEADER;
	}
	if (strcasecmp(http_str, "OK")!=0) {
		return BAD_RESPONSE_HEADER;
	}
	checkstate = CHECK_STATE_HEADER;
	return NEED_RESPONSE_HEADER;
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
HTTP_REQUEST_CODE parse_request_headers( char* szTemp )
{
	int all;
	char *tmp;
	if ( szTemp[ 0 ] == '\0' )
	{
		return GET_REQUEST_HEADER;
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
	return NEED_RESQUEST_HEADER;
}

HTTP_RESPONSE_CODE parse_response_headers( char* szTemp )
{
	int all;
	char *tmp;
	if ( szTemp[ 0 ] == '\0' )
	{
		return GET_RESPONSE_HEADER;
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
	return NEED_RESPONSE_HEADER;
}

HTTP_REQUEST_CODE parse_content( char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line, char **ret_name )
{
	LINE_STATUS linestatus = LINE_OK;
	HTTP_REQUEST_CODE retcode = NEED_RESQUEST_HEADER;
	char *name = (char *)malloc(50);

	while( ( linestatus = parse_line( buffer, checked_index, read_index ) ) == LINE_OK )
	{
		printf("--OK--:");
		char* szTemp = buffer + start_line;
		start_line = checked_index;
		switch ( checkstate )
		{
			case CHECK_STATE_LINE:
			{
				bzero(name, strlen(name));
				retcode = parse_requestline( szTemp, checkstate, &name);
				if ( retcode == BAD_REQUEST_HEADER )
				{
					return BAD_REQUEST_HEADER;
				}
				printf(">>>%s\n", name);
				*ret_name = strdup(name);
				break;
			}
			case CHECK_STATE_HEADER:
			{
				retcode = parse_request_headers( szTemp );
				if ( retcode == BAD_REQUEST_HEADER )
				{
					return BAD_REQUEST_HEADER;
				}
				else if ( retcode == GET_REQUEST_HEADER )
				{
					return GET_REQUEST_HEADER;
				}
				break;
			}
			default:
			{
				return INTERNAL_REQUEST_ERROR;
			}
		}
	}
	if( linestatus == LINE_OPEN )
	{
		return NEED_RESQUEST_HEADER;
	}
	else
	{
		return BAD_REQUEST_HEADER;
	}
	free(name);
}

HTTP_RESPONSE_CODE parse_response_content( char* buffer, int& checked_index, \
		CHECK_STATE& checkstate, int& read_index, int& start_line, int *content_length)
{
	LINE_STATUS linestatus = LINE_OK;
	HTTP_RESPONSE_CODE retcode = NEED_RESPONSE_HEADER;

	while( ( linestatus = parse_line( buffer, checked_index, read_index ) ) == LINE_OK )
	{
		char* szTemp = buffer + start_line;
		start_line = checked_index;
		switch ( checkstate )
		{
			case CHECK_STATE_LINE:
			{
				retcode = parse_responseline(szTemp, checkstate);
				if ( retcode == BAD_RESPONSE_HEADER )
				{
					return BAD_RESPONSE_HEADER;
				}
				break;
			}
			case CHECK_STATE_HEADER:
			{
				retcode = parse_response_headers( szTemp );//TODO content_length
				if ( retcode == BAD_RESPONSE_HEADER )
				{
					return BAD_RESPONSE_HEADER;
				}
				else if ( retcode == GET_RESPONSE_HEADER )
				{
					return GET_RESPONSE_HEADER;
				}
				break;
			}
			default:
			{
				return INTERNAL_RESPONSE_ERROR;
			}
		}
	}
	if( linestatus == LINE_OPEN )
	{
		return NEED_RESPONSE_HEADER;
	}
	else
	{
		return BAD_RESPONSE_HEADER;
	}
}
