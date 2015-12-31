#ifndef CLIENT_H__
#define CLIENT_H__

#define DEFAULT_PLAYER		"/usr/bin/mpg123"

struct client_conf_st
{
	char *mgroup;
	char *rcvport;
	char *player;
};

extern struct client_conf_st client_conf;

#endif


