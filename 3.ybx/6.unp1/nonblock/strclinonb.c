#include "unp.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE 4096

void str_cli(FILE *fp, int sockfd)
{
	int n;
	int val, stdineof, maxfdp1;
	fd_set rset, wset;
	char to[MAXLINE], fr[MAXLINE];
	char *toiptr, *tooptr, *friptr, *froptr;

	val = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, val|O_NONBLOCK);

	val = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, val|O_NONBLOCK);

	val = fcntl(STDOUT_FILENO, F_GETFL, 0);
	fcntl(STDOUT_FILENO, F_SETFL, val|O_NONBLOCK);

	toiptr = tooptr = to;
	friptr = froptr = fr;
	stdineof = 0;

	maxfdp1 = max(max(STDIN_FILENO, STDOUT_FILENO), sockfd)+1;

	while (1) {
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		if (stdineof==0 && toiptr < &to[MAXLINE]){
			FD_SET(STDIN_FILENO, &rset);
		}
		if (friptr < &fr[MAXLINE]){
			FD_SET(sockfd, &rset);
		}
		if (tooptr != toiptr){
			FD_SET(sockfd, &wset);
		}
		if (froptr != friptr){
			FD_SET(STDOUT_FILENO, &wset);
		}

		select(maxfdp1, &rset, &wset, NULL, NULL);

		//1.从标准输入写入to buffer
		if (FD_ISSET(STDIN_FILENO, &rset)) {
			n = read(STDIN_FILENO, toiptr, (&to[MAXLINE]-toiptr));
			if (n<0) {
				if (errno!=EAGAIN) {
					perror("EAGIAN");
					exit(1);
				}
			}else if(n==0){
				fprintf(stderr, "%s:EOF on stdin\n", gf_time());
				stdineof=1;
				//标准输入EOF结束并且to buffer已经没数据
				//关闭socket 写端
				if (tooptr == toiptr ) {
					shutdown(sockfd, SHUT_WR);
				}
			}else{
				fprintf(stderr, "%s:read %d bytes from stdin\n", gf_time(), n);
				toiptr+=n;
			}
		}

		//2.从to buffer写入到socket
		if (FD_ISSET(sockfd, &wset) && ((toiptr-tooptr)>0)) {
			n = write(sockfd, tooptr, toiptr-tooptr);
			if (n<0) {
				if (errno!=EAGAIN) {
					perror("write");
					exit(1);
				}
			}else{
				fprintf(stderr, "%s:write %d bytes to socket\n", gf_time(), n);
				tooptr+=n;
				//标准输入EOF结束并且to buffer已经没数据
				//关闭socket 写端
				if (tooptr == toiptr) {
					tooptr = toiptr = to;
					if (stdineof) {
						shutdown(sockfd, SHUT_WR);
					}
				}
			}
		}

		//3.从scoket读数据到 fd buffer
		if (FD_ISSET(sockfd, &rset)) {
			n = read(sockfd, friptr, (&fr[MAXLINE]-friptr));
			if (n<0) {
				if (errno!=EAGAIN) {
					perror("EAGIAN");
					exit(1);
				}
			}else if(n==0){
				fprintf(stderr, "%s:close on socket\n", gf_time());
				return;
			}else{
				fprintf(stderr, "%s:read %d bytes from socket\n", gf_time(), n);
				friptr+=n;
			}
		}

		//4.从fr buferr写取数据到stdout
		if (FD_ISSET(STDOUT_FILENO, &wset) && ((friptr-froptr)>0)) {
			n = write(STDOUT_FILENO, froptr, friptr-froptr);
			if (n<0) {
				if (errno!=EAGAIN) {
					perror("write");
					exit(1);
				}
			}else{
				fprintf(stderr, "%s:write %d bytes to stdout\n", gf_time(), n);
				froptr+=n;
				if (froptr == friptr) {
					froptr = friptr = fr;
				}
			}
		}
	}
}
