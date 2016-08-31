#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH		"/etc/hosts"
#define KEYPROJ		'C'

#define PATHSIZE	1024
#define DATASIZE	1024

enum{
	STATE_RCV=1,
	STATE_SEND,
	STATE_READ,
	STATE_OUT,
	STATE_EX,
	STATE_T
};

enum
{
	MSG_PATH=1,
	MSG_DATA,
	MSG_ERR,
	MSG_EOT
};

typedef struct msg_path_st
{
	long mtype;				/*must be MSG_PATH*/
	char path[PATHSIZE];	/*ASCIIZ带尾0的串*/
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
	}s2c;
}msg_s2c_t;

#endif



