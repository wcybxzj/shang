#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<string.h>
#include<errno.h>

#include<proto.h>

#include "server.h"
#include "thr_channle.h"
#include "medialib.h"


#define BUFSIZE 	1024

void * send_data(void* id)
{
	int len;//getchndata's return;
	char buf[BUFSIZE];
	struct msg_chnnal_st *sbuf;
	struct sockaddr_in daddr;
	int size;

	chnid_t chnid = *(chnid_t *)id;

	while(1)
	{
		len = mlib_getchndata(chnid,buf,BUFSIZE);
		if(len < 0)
		{
			perror("getchndata()");
			pthread_exit(NULL);
		}
/*		if(len == 0)//data ask finished 
		{
			break;
		}*/
		else if(len > 0)
		{
			size = sizeof(*sbuf)+len-1;
			sbuf = malloc(size);
			sbuf->chnid = chnid;
			strncpy(sbuf->data,buf,len);
			
			daddr.sin_family = AF_INET;
			daddr.sin_port = htons(atoi(DEFAULT_RCVPORT));
			inet_pton(AF_INET,DEFAULT_MGROUP,&daddr.sin_addr);

			while(sendto(sd,sbuf,size,0,(void *)&daddr,sizeof(daddr)) < 0)
			{
				if(errno == EINTR)
				{
					continue;	
				}
				else
				{
					perror("sendto()");
					free(sbuf);
					sbuf = NULL;
					pthread_exit(NULL);
				}
			}
			free(sbuf);
		}
	}
}

int thr_channel_create(chnid_t *chnid)
{
	int err;
/*
 *sendto()构造send函数再send中构建socket;
 *pthread_create()创建线程
 *servre.c中包含pthread_t tid_arr[NR_CHN]
 * */
	err = pthread_create(&tid[*chnid],NULL,send_data,(void *)chnid);
	if(err)
	{
		fprintf(stderr,"pthread_create():%s\n",strerror(err));
		return -1;
	}
	return 0;
}



int thr_channel_destroy(chnid_t chnid)
{
/*
 *pthread_join 收回线程  
 * */
	pthread_join(tid[chnid],NULL);
}

int thr_channel_destroyall(int listsize)
{
/*
 *pthread_join 收回线程 
 * */
	int i;
	for(i = 1 ; i <= listsize ; i++)
	{
		pthread_join(tid[i],NULL);
	}
}
