#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>

#define SIZE 1024

#define DELIM " \n\t"

typedef struct _cmd_st {
	glob_t res;
} cmd_st;

static void prompt()
{
	printf("dabing@x220$");
}

static void parse(char **str,  cmd_st *cmd){
	char *tok;
	int i = 0;
	while (1) {
		tok = strsep(str, DELIM);
		if (tok == NULL) {
			break;
		}
		if (strcmp(tok, "\0") ==0 ||
			strcmp(tok, "\t") ==0 || 
			strcmp(tok, "\n")==0){
			continue;
		}
		//printf("%s\n", tok);
		glob(tok, GLOB_NOCHECK|GLOB_APPEND*i, NULL, &cmd->res);
		i = 1;
	}
}

int main(int argc, const char *argv[])
{
	pid_t pid;
	int i;
	char *str = NULL;
	int size = 0;
	cmd_st cmd;

	while (1) {
		prompt();

		getline(&str, &size, stdin);

		parse(&str ,&cmd);

		//for (i = 0; i < cmd.res.gl_pathc; i++) {
		//	printf("%s\n", cmd.res.gl_pathv[i]);
		//}

		if (0) {//内部命令

		}else{//外部命令
			pid = fork();
			if (pid < 0) {
				perror("fork()");
				exit(1);
			}
			if (pid == 0) {
				execvp(cmd.res.gl_pathv[0], cmd.res.gl_pathv);
			}
			wait(NULL);
		}
	}


	return 0;
}
