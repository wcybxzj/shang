#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH		"/etc/hosts"
#define KEYPROJ		'C'

#define PATHSIZE	1024
#define DATASIZE	1024

struct msg_path_st
{
	char path[PATHSIZE];	/*ASCIIZ带尾0的串*/
};

struct msg_data_st
{
	int datalen;
	char data[DATASIZE];
};

struct msg_err_st
{
	int _errno_;
};

struct msg_eot_st
{

};

#endif



