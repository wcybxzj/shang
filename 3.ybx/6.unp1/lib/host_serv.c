#include "unp.h"

struct addrinfo * host_serv(const char *host, const char *serv, \
		int family, int socktype){
	int n;
	struct addrinfo hints, *res;

	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;

	n = getaddrinfo(host, serv, &hints, &res);
	if (n != 0) {
		return NULL;
	}else{
		return res;
	}
}


struct addrinfo * Host_serv(const char *host, const char *serv, \
		int family, int socktype){
	int n;
	struct addrinfo hints, *res;

	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;

	n = getaddrinfo(host, serv, &hints, &res);
	if (n != 0) {
		err_quit("Host_serv error for %s,%s:%s",
				(host == NULL)? "no hostname": host,
				(serv == NULL)? "no service name": serv,
				gai_strerror(n));
	}else{
		return res;
	}
}
