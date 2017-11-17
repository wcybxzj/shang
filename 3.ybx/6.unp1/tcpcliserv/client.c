#include    "unp.h"

#define MAXBUF 8096000
//#define MAXBUF 40960

void processSignal(int signo)
{
	printf("Signal is %d\n", signo);
	signal(signo, processSignal);
}

void str_cli(FILE *fp, int sockfd)
{
	char  sendline[MAXBUF];

	while (1) {
		memset(sendline, 'a', sizeof(sendline));
		printf("Begin send %d data\n", MAXBUF);
		Writen(sockfd, sendline, sizeof(sendline));
		printf("client:sleep 5\n");
		sleep(5);
	}
}

//在看hplsp的时候研究 13-4shm_talk_server.cpp 对其中SIGPIPE的处理不理解
//查了下资料
//http://www.cppblog.com/elva/archive/2008/09/10/61544.html
int main(int argc, char **argv)
{
	int                    sockfd;
	struct sockaddr_in    servaddr;

	signal(SIGPIPE, SIG_IGN);
	//signal(SIGPIPE, processSignal);

	if (argc != 3)
		err_quit("usage: ./a.out ip port");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);        /* do it all */

	exit(0);
}
