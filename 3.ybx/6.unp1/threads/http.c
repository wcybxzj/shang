#include "http.h"

void deal_url(char *url , char **name){
	*name = strdup("index.html");
	if (strcmp(url, "/") != 0) {
		url++;
		*name = url;
	}
}

enum LINE_STATUS parse_line( char *buffer, int *checked_index, int read_index )
{
	char temp;
	for ( ; *checked_index < read_index; ++(*checked_index) )
	{
		temp = buffer[ *checked_index ];
		if ( temp == '\r' )
		{
			if ( ( *checked_index + 1 ) == read_index )
			{
				return LINE_OPEN;
			}
			else if ( buffer[ *checked_index + 1 ] == '\n' )
			{
				buffer[ (*checked_index)++ ] = '\0';
				buffer[ (*checked_index)++ ] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if( temp == '\n' )
		{
			if( ( *checked_index > 1 ) &&  buffer[ *checked_index - 1 ] == '\r' )
			{
				buffer[ *checked_index-1 ] = '\0';
				buffer[ (*checked_index)++ ] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
	}
	return LINE_OPEN;
}

//HTTP/1.1 200 OK
enum HTTP_RESPONSE_CODE parse_responseline(char* szTemp, enum CHECK_STATE *checkstate)
{
	char* szURL = strpbrk( szTemp, " \t" );
	//printf( "The request URL is: %s\n", szTemp );
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
	*checkstate = CHECK_STATE_HEADER;
	return NEED_RESPONSE_HEADER;
}

enum HTTP_RESPONSE_CODE parse_response_headers( char* szTemp, int *content_length)
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
			if (strcasecmp("Content-Length", szTemp)==0) {
				*content_length = atoi(tmp);
			}
			//printf("%s:%s\n", szTemp,tmp);
		}
		else
		{
			printf( "I can not handle this header\n" );
		}
	}
	return NEED_RESPONSE_HEADER;
}

enum HTTP_RESPONSE_CODE parse_response_content(char* buffer, int *checked_index, \
		enum CHECK_STATE *checkstate, int read_index, int *start_line, \
		int *content_length)
{
	enum LINE_STATUS linestatus = LINE_OK;
	enum HTTP_RESPONSE_CODE retcode = NEED_RESPONSE_HEADER;

	while((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK )
	{
		char* szTemp = buffer + (*start_line);
		*start_line = *checked_index;
		switch ( *checkstate )
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
				retcode = parse_response_headers( szTemp, content_length);
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
