#include "server.h"

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>
#include "intset.h"


int main(int argc, char *argv[])
{
	struct timeval tv;
	int j;

#ifdef INIT_SETPROCTITLE_REPLACEMENT
	spt_init(argc, argv);
#endif
	setlocale(LC_COLLATE,"");
	zmalloc_enable_thread_safeness();
	zmalloc_set_oom_handler(redisOutOfMemoryHandler);
	srand(time(NULL)^getpid());
	gettimeofday(&tv,NULL);
	dictSetHashFunctionSeed(tv.tv_sec^tv.tv_usec^getpid());
	server.sentinel_mode = checkForSentinelMode(argc,argv);
	initServerConfig();

	/* Store the executable path and arguments in a safe place in order
	 * to be able to restart the server later. */
	server.executable = getAbsolutePath(argv[0]);
	server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
	server.exec_argv[argc] = NULL;
	for (j = 0; j < argc; j++) server.exec_argv[j] = zstrdup(argv[j]);

	//哨兵

	//redis-check-rdb

	if (argc >= 2) {

	} else {
		serverLog(LL_WARNING,\
				"Warning: no config file specified, using the default config. \
				In order to specify a config file use %s /path/to/%s.conf", \
				argv[0], server.sentinel_mode ? "sentinel" : "redis");
	}

	server.supervised = redisIsSupervised(server.supervised_mode);
	int background = server.daemonize && !server.supervised;
	if (background) daemonize();


	if (argc >= 2) {
		j = 1; /* First option to parse in argv[] */
		sds options = sdsempty();
		char *configfile = NULL;

		/* Handle special options --help and --version */
		if (strcmp(argv[1], "-v") == 0 ||
				strcmp(argv[1], "--version") == 0) version();
		if (strcmp(argv[1], "--help") == 0 ||
				strcmp(argv[1], "-h") == 0) usage();

		/* First argument is the config file name? */
		if (argv[j][0] != '-' || argv[j][1] != '-') {
			configfile = argv[j];
			server.configfile = getAbsolutePath(configfile);
			/* Replace the config file in server.exec_argv with
			 *              * its absoulte path. */
			zfree(server.exec_argv[j]);
			server.exec_argv[j] = zstrdup(server.configfile);
			j++;
		}

		while(j != argc) {
			if (argv[j][0] == '-' && argv[j][1] == '-') {
				/* Option name */
				if (!strcmp(argv[j], "--check-rdb")) {
					/* Argument has no options, need to skip for parsing. */
					j++;
					continue;
				}
				if (sdslen(options)) options = sdscat(options,"\n");
				options = sdscat(options,argv[j]+2);
				options = sdscat(options," ");
			} else {
				/* Option argument */
				options = sdscatrepr(options,argv[j],strlen(argv[j]));
				options = sdscat(options," ");
			}
			j++;
		}
		if (server.sentinel_mode && configfile && *configfile == '-') {
			serverLog(LL_WARNING,
					"Sentinel config from STDIN not allowed.");
			serverLog(LL_WARNING,
					"Sentinel needs config file on disk to save state.  Exiting...");
			exit(1);
		}

		resetServerSaveParams();
		loadServerConfig(configfile,options);
		sdsfree(options);

	}else{
		serverLog(LL_WARNING, "Warning: no config file specified, using the default config. In order to specify a config file use %s /path/to/%s.conf", argv[0], server.sentinel_mode ? "sentinel" : "redis");
	}


	// 初始化服务器
	initServer();
	// 创建保存pid的文件
	if (background || server.pidfile) createPidFile();
	// 为服务器进程设置标题
	redisSetProcTitle(argv[0]);
	redisAsciiArt();
	// 检查backlog队列
	checkTcpBacklogSettings();


	/* Warning the user about suspicious maxmemory setting. */
	// 最大内存限制是否配置正确
	if (server.maxmemory > 0 && server.maxmemory < 1024*1024) {
		serverLog(LL_WARNING,"WARNING: You specified a maxmemory value that is less than 1MB (current value is %llu bytes). Are you sure this is what you really want?", server.maxmemory);
	}

	if (!server.sentinel_mode) {
		/* Things not needed when running in Sentinel mode. */
		serverLog(LL_WARNING,"Server started, Redis version " REDIS_VERSION);
		loadDataFromDisk();
		if (server.cluster_enabled) {
			//TODO
		}
		if (server.ipfd_count > 0)
			serverLog(LL_NOTICE,"The server is now ready to accept connections on port %d", server.port);
		if (server.sofd > 0)
			serverLog(LL_NOTICE,"The server is now ready to accept connections at %s", server.unixsocket);
	} else {
		//TODO
	}


	aeSetBeforeSleepProc(server.el,beforeSleep);
	aeMain(server.el);
	aeDeleteEventLoop(server.el);

	return 0;
}
