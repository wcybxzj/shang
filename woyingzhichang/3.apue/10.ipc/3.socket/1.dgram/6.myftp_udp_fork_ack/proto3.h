#ifndef PROTO_H__
#define PROTO_H__

//因为没有类型的数据是不一定的
//所以这里为了保证最后使用的int 
//做成字符串用atoi转成int
#define RCVPORT "1990"

#define MSGMAX (512-20-8)

#define PATHSIZE	(MSGMAX-8)
#define DATASIZE	(MSGMAX-8-4-4)

enum
{
	MSG_DATA=1,
	MSG_ACK,
	MSG_EOT,
	MSG_ERR,
	MSG_PATH
};

typedef struct msg_path_st
{
	char path[PATHSIZE];
}msg_path_t;

typedef struct msg_data_st
{
	int datalen;//4
	int data_num;//4
	char data[DATASIZE];
}msg_data_t;

typedef struct msg_err_st
{
	int _errno_;
}msg_err_t;

typedef struct msg_eot_st
{
}msg_eot_t;

typedef struct msg_ack_st
{
	int ack_data_num;
}msg_ack_t;

typedef struct msg_s2c_st
{
	long mtype;//8
	union msg_s2c_un
	{
		msg_path_t pathmsg;
		msg_data_t datamsg;
		msg_err_t errmsg;
		msg_eot_t eotmsg;
		msg_ack_t ackmsg;
	}data;
}msg_s2c_t;

#endif



