#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define STRSIZE		1024

/*
[root@web11 4.sys]# ./5.mydate -y 4
2017
[root@web11 4.sys]# ./5.mydate -a
./5.mydate: invalid option -- 'a'
不能识别的选项是:a
*/
int main(int argc,char **argv)
{
	time_t stamp;
	struct tm *tm;
	char timestr[STRSIZE];
	int ch;
	char fmtstr[STRSIZE] = {'\0'};
	FILE *fp = stdout;

	stamp = time(NULL);
	tm = localtime(&stamp);

	while(1)
	{
		//printf("==optind:%d===\n", optind);
		ch = getopt(argc,argv,"-y:mdH:MS");
		//printf("==optind:%d===\n", optind);
		if(ch < 0)
			break;

		switch(ch)
		{
			case 1:
				//printf("%s\n", argv[optind-1]);
				if(fp == stdout)
				{
					fp = fopen(argv[optind-1],"w");
					if(fp == NULL)
					{
						perror("fopen()");
						fp = stdout;
					}
				}
				break;

			case 'y':
				if(strcmp(optarg,"2") == 0)
					strncat(fmtstr,"%y ",STRSIZE);
				else if(strcmp(optarg,"4") == 0)
					strncat(fmtstr,"%Y ",STRSIZE);
				else
					fprintf(stderr,"Invalid argument of -y\n");
				break;

			case 'm':
				strncat(fmtstr,"%m ",STRSIZE);
				break;

			case 'd':
				strncat(fmtstr,"%d ",STRSIZE);
				break;

			case 'H':
				if(strcmp(optarg,"12") == 0)
					strncat(fmtstr,"%I(%P) ",STRSIZE);
				else if(strcmp(optarg,"24") == 0)
					strncat(fmtstr,"%H ",STRSIZE);
				else
					fprintf(stderr,"Invalid argument of -H\n");

				break;

			case 'M':
				strncat(fmtstr,"%M ",STRSIZE);
				break;

			case 'S':
				strncat(fmtstr,"%S ",STRSIZE);
				break;

			//有了这个就不用下面default了
			//并且optopt会指向导致出错的选项字符串
			case '?':
					fprintf(stderr,"不能识别的选项是:%c",optopt);
					break;

			//default:
			//	//			fprintf(stderr,"Invalid argument.");
			//	//			_exit(1);
			//	//			abort();
			//	break;
		}

	}

	strncat(fmtstr,"\n",STRSIZE);
	strftime(timestr,STRSIZE,fmtstr,tm);
	fputs(timestr,fp);

	if(fp != stdout)
		fclose(fp);

	exit(0);
}


