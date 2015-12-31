#ifndef SERVER_H__
#define SERVER_H__


struct server_conf_st {
	char *mgroup;
	char *rcvport;
	char *medpath;	
};

extern int sd;
extern struct sockaddr_in raddr;
extern pthread_t tid[NR_CHN+1];
extern struct server_conf_st server_conf;

#endif
