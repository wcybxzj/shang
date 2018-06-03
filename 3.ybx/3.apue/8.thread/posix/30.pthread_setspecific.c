#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define MAXSTRINGSZ 4096

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void
thread_init(void)
{
	pthread_key_create(&key, free);
}

char *
getenv(const char *name)
{
	int		i, len;
	char	*envbuf;

	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);
	envbuf = (char *)pthread_getspecific(key);
	if (envbuf == NULL) {
		printf("NULL\n");//有几个线程就运行几次
		envbuf = malloc(MAXSTRINGSZ);
		if (envbuf == NULL) {
			pthread_mutex_unlock(&env_mutex);
			return(NULL);
		}
		pthread_setspecific(key, envbuf);
	}
	len = strlen(name);
	for (i = 0; environ[i] != NULL; i++) {
		if ((strncmp(name, environ[i], len) == 0) &&
		  (environ[i][len] == '=')) {
			strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
			pthread_mutex_unlock(&env_mutex);
			return(envbuf);
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return(NULL);
}

static void *func(void*arg)
{
	char *str = getenv("LANG");
	printf("%s\n",str);
}

/*
[root@web11 posix]# ./30.pthread_setspecific 
NULL
en_US.UTF-8
NULL
en_US.UTF-8
NULL
en_US.UTF-8
NULL
en_US.UTF-8
*/

int main(int argc, const char *argv[])
{
	int i, num=4, err;
	pthread_t tids[num];
	for (i = 0; i < num; i++) {
		err = pthread_create(&tids[i],NULL,func,NULL);
		if (err) {
			fprintf(stderr, "%s\n", strerror(err));
			exit(1);
		}
	}


	for (i = 0; i < num; i++) {
		pthread_join(tids[i], NULL);
	}
	return 0;
}
