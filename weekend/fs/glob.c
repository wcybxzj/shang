#include <stdio.h>
#include <stdlib.h>
#include <glob.h>


#define PAT		"/etc/a*.conf"

int  err_path (const char *epath, int eerrno)
{
	puts(epath);
	fprintf(stderr,"err_path():%s\n",strerror(eerrno));
	return 0;
}

int main()
{
	glob_t globres;
	int i,err;

	err = glob(PAT,0,NULL/*err_path*/,&globres);
	if(err)
	{
		printf("glob()error:%d\n",err);
		exit(1);
	}

	for(i = 0 ; i < globres.gl_pathc; i++)
		puts(globres.gl_pathv[i]);

	globfree(&globres);
	
	exit(0);
}


