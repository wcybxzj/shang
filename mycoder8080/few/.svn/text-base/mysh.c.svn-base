#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>

#define DELIMS		" \t\n"

struct cmd_st
{
	glob_t globres;
};

static void prompt(void)
{
	printf("mysh-0.1$ ");
}

static void parse(char *line,struct cmd_st *res)
{// ls     -l    /etc   
	char *tok;
	int i = 0;

	while(1)
	{
		tok = strsep(&line,DELIMS);
		if(tok == NULL)
			break;
		if(tok[0] == '\0')
			continue;

		glob(tok,GLOB_NOCHECK|GLOB_APPEND*i,NULL,&res->globres);
		/*if error*/
		i = 1;
	}

}

int main()
{
	char *line = NULL;
	size_t line_size = 0;
	struct cmd_st cmd;
	pid_t pid;

	while(1)
	{
		prompt();
		
		getline(&line,&line_size,stdin);
		/*if error*/

		parse(line,&cmd);

		if(0/*是内部命令*/)
		{	}
		else	//外部命令处理
		{
			pid = fork();
			if(pid < 0)
			{
				perror("fork()");	
				exit(1);
			}
	
			if( pid == 0)
			{			
				execvp(cmd.globres.gl_pathv[0],cmd.globres.gl_pathv);
				perror("execvp()");
				exit(1);
			}	
			else
				wait(NULL);	
		}
	}
	
	exit(0);
}

