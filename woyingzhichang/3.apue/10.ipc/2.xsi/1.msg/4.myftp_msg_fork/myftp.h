#include "proto3.h"

enum{
	STATE_RCV=1,
	STATE_SEND,
	STATE_READ,
	STATE_OUTPUT,
	STATE_EX,
	STATE_T
};


typedef struct fsm_st{
	int state;
	int server_id;//服务端msg id
	int fd;
	char *errstr;
	msg_path_t path_buf;
	msg_s2c_t data_buf;
} FSM_ST;
