#include "apue.h"
#define MAXARGC 50
#define WHITE " \t\n"

int
buf_args(char *buf, int(*func)(int, char **))
{
	char *tmp;
	int argc = 0;
	char *argv[MAXARGC];
	if ( (tmp = strtok(buf, WHITE))==NULL ) {/*argv[0] 必须存在*/
		/*strcpy(errmsg, "request must end with NULL");*/
		return -1;
	}

	/*两种方式都可以*/
	/*argv[argc] = tmp;*/
	argv[argc] = buf;

	while ( (tmp = strtok(NULL, WHITE)) != NULL ) {
		argc++;
		if(argc>=MAXARGC-1){
			/*strcpy(errmsg, "request cmd number is too many");*/
			return -1;
		}
		argv[argc] = tmp;
	}

	argv[++argc] = NULL;

	return (*func)(argc, argv);
}
