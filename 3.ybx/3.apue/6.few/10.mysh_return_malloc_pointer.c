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

//返回值为指向堆的指针
static cmd_st* parse_return_malloc_pointer(char *str){
	cmd_st *cmd;
	cmd = malloc(sizeof(cmd_st));
	if (NULL == cmd) {
		perror("malloc():");
		exit(1);
	}
	char *tok;
	int i = 0;
	while (1) {
		if (i == 0) {
			tok = strtok(str, DELIM);
		}else{
			tok = strtok(NULL, DELIM);
		}
		if (tok == NULL) {
			break;
		}
		//printf("%s\n", tok);
		glob(tok, GLOB_NOCHECK|GLOB_APPEND*i, NULL, &cmd->res);
		i = 1;
	}
	return cmd;
}

int main(int argc, const char *argv[])
{
	pid_t pid;
	int i;
	char *str = NULL;
	int size = 0;
	cmd_st *cmd_p;

	while (1) {
		prompt();

		getline(&str, &size, stdin);

		cmd_p = parse_return_malloc_pointer(str);

		for (i = 0; i < cmd_p->res.gl_pathc; i++) {
			printf("%s\n", cmd_p->res.gl_pathv[i]);
		}

		if (0) {//内部命令

		}else{//外部命令
			pid = fork();
			if (pid < 0) {
				perror("fork()");
				exit(1);
			}
			if (pid == 0) {
				execvp(cmd_p->res.gl_pathv[0], cmd_p->res.gl_pathv);
			}
			free(cmd_p);
			wait(NULL);
		}
	}

	return 0;
}
