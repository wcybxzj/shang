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

enum HTTP_RESPONSE_CODE parse_response_content( char* buffer, \
		int *checked_index,
		enum CHECK_STATE *checkstate, \
		int read_index, \
		int *start_line, \
		int *content_length);
