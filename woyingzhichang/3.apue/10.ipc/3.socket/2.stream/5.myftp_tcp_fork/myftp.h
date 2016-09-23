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
	//struct sockaddr_in client_addr, server_addr;
	//socklen_t client_addr_len;
	//int server_child_sd;//server client sd
	//int client_sd;
	int fd;
	int newsd;
	char *errstr;
	msg_path_t path_buf;
	msg_s2c_t data_buf;
} FSM_ST;
