#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


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
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_child_addr;
	socklen_t client_addr_len;
	socklen_t server_child_addr_len;
	int server_child_sd;//server client sd
	int server_sd;//server sd
	int client_sd;
	int fd;
	char *errstr;
	msg_s2c_t data_buf;
} FSM_ST;
