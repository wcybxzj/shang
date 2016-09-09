#ifndef PROTO_H__
#define PROTO_H__

#define SERVER_KEY 0x00000001

#define PATHSIZE	1024
#define DATASIZE	1024


enum
{
	MSG_DATA=1,
	MSG_EOT,
	MSG_ERR,
	MSG_PATH
};

typedef struct msg_path_st
{
	long mtype;
	int client_id;//客户端msg id
	char path[PATHSIZE];
}msg_path_t;

typedef struct msg_data_st
{
	int datalen;
	char data[DATASIZE];
}msg_data_t;

typedef struct msg_err_st
{
	int _errno_;
}msg_err_t;

typedef struct msg_eot_st
{
}msg_eot_t;

typedef struct msg_s2c_st
{
	long mtype;
	union msg_s2c_un
	{
		msg_data_t datamsg;
		msg_err_t errmsg;
		msg_eot_t eotmsg;
	}data;
}msg_s2c_t;

#endif



