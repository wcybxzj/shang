#include "apue.h"
#include <sys/socket.h>
#define CONTROLLEN CMSG_LEN(sizeof(int))

static struct cmsghdr *cmptr = NULL;

int recv_fd(int fd, ssize_t (*userfunc)(int, const void *,size_t))
{
	int nr;
	int newfd;
	int status;
	char *ptr;
	char buf[MAXLINE];
	struct iovec iov[1];
	struct msghdr msg;

	status = -1;
	while (1) {
		/*准备*/
		iov[0].iov_base = buf;
		iov[0].iov_len = sizeof(buf);
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		if (cmptr == NULL){
			if( (cmptr = malloc(CONTROLLEN)) == NULL) {
				return(-1);
			}
		}
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;
		/*接受数据*/
		nr =recvmsg(fd, &msg, 0);
		if (nr<0) {
			err_ret("recvmsg error");
			return(-1);
		}else if(nr== 0){
			err_ret("server side close");
			return(-1);
		}

		/*
		分析二个字节协议
		协议= buf为正常数据+辅助数据
		buf[0]=0 buf[1]=正数 说明发送端有错误
		buf[0]=0 buf[1]=0    说明发送端正确 ,获取辅助数据中的fd
		*/
		/*
		这个代码精妙之处在于接受协议前可以有内容
		例如:buf:aasdadsadadd00,只有接受到一个0才开始识别协议
		*/
		for ( ptr = buf; ptr <&buf[nr] ; ) {
			/*第一个字节*/
			if (*ptr++==0) {
				/*要求协议二个字节是整体buf的末尾*/
				if(ptr!=&buf[nr-1]){
					err_dump("协议格式有问题");
				}
				/*第二个字节控制范围(0-255), 实际只有0或者1*/
				status = *ptr & 0XFF;
				if (status==0) {
					if (msg.msg_controllen < CONTROLLEN) {
						err_dump("status=0,but not have fd");
					}
					newfd = *(int *)CMSG_DATA(cmptr);
				}else{
					newfd = -status;/*获取errno*/
				}
				nr -= 2;/*消费了2个字节*/
			}
		}
		/*情况1:整个buf,没有协议的情况*/
		/*情况2:buf中的协议之前有大量数据*/
		/*打印buf中的非协议部分到客户端的STDERR*/
		if (nr>0) {
			if ( (*userfunc)(STDERR_FILENO, buf, nr)!=nr ){
				return -1;
			}
		}
		if (status >= 0) {
			/*正常的fd 或者 -status(-1)*/
			return newfd;
		}
	}
}
