#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH		"/etc/hosts"
#define KEYPROJ		'C'

#define PATHSIZE	1024
#define DATASIZE	1024

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
	long mtype;				/*must be MSG_DATA*/
	int datalen;
	char data[DATASIZE];
}msg_data_t;

typedef struct msg_err_st
{
	long mtype;				/*must be MSG_ERR*/
	int _errno_;
}msg_err_t;

typedef struct msg_eot_st
{
	long mtype;				/*must be MSG_EOT*/
}msg_eot_t;

union msg_s2c_un
{
	long mtype;
	msg_data_t datamsg;
	msg_err_t errmsg;
	msg_eot_t eotmsg;
};


#endif



