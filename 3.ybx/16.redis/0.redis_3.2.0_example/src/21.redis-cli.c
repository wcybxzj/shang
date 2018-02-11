#include "fmacros.h"
#include "version.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>

#include <hiredis.h>
#include <sds.h> /* use sds.h from hiredis, so that only one set of sds functions will be present in the binary */
#include "zmalloc.h"
#include "linenoise.h"
#include "help.h"
#include "anet.h"
#include "ae.h"

#define UNUSED(V) ((void) V)

#define OUTPUT_STANDARD 0
#define OUTPUT_RAW 1
#define OUTPUT_CSV 2
#define REDIS_CLI_KEEPALIVE_INTERVAL 15 /* seconds */
#define REDIS_CLI_DEFAULT_PIPE_TIMEOUT 30 /* seconds */
#define REDIS_CLI_HISTFILE_ENV "REDISCLI_HISTFILE"
#define REDIS_CLI_HISTFILE_DEFAULT ".rediscli_history"
#define REDIS_CLI_RCFILE_ENV "REDISCLI_RCFILE"
#define REDIS_CLI_RCFILE_DEFAULT ".redisclirc"

/* --latency-dist palettes. */
int spectrum_palette_color_size = 19;
int spectrum_palette_color[] = {0,233,234,235,237,239,241,243,245,247,144,143,142,184,226,214,208,202,196};

int spectrum_palette_mono_size = 13;
int spectrum_palette_mono[] = {0,233,234,235,237,239,241,243,245,247,249,251,253};

/* The actual palette in use. */
int *spectrum_palette;
int spectrum_palette_size;

static redisContext *context;
static struct config {
    char *hostip;
    int hostport;
    char *hostsocket;
    long repeat;
    long interval;
    int dbnum;
    int interactive;
    int shutdown;
    int monitor_mode;
    int pubsub_mode;
    int latency_mode;
    int latency_dist_mode;
    int latency_history;
    int lru_test_mode;
    long long lru_test_sample_size;
    int cluster_mode;
    int cluster_reissue_command;
    int slave_mode;
    int pipe_mode;
    int pipe_timeout;
    int getrdb_mode;
    int stat_mode;
    int scan_mode;
    int intrinsic_latency_mode;
    int intrinsic_latency_duration;
    char *pattern;
    char *rdb_filename;
    int bigkeys;
    int stdinarg; /* get last arg from stdin. (-x option) */
    char *auth;
    int output; /* output mode, see OUTPUT_* defines */
    sds mb_delim;
    char prompt[128];
    char *eval;
    int eval_ldb;
    int eval_ldb_sync;  /* Ask for synchronous mode of the Lua debugger. */
    int eval_ldb_end;   /* Lua debugging session ended. */
    int enable_ldb_on_eval; /* Handle manual SCRIPT DEBUG + EVAL commands. */
    int last_cmd_type;
} config;

/* User preferences. */
static struct pref {
    int hints;
} pref;

static volatile sig_atomic_t force_cancel_loop = 0;
static void usage(void);
static void slaveMode(void);
char *redisGitSHA1(void);
char *redisGitDirty(void);
static int cliConnect(int force);

/*------------------------------------------------------------------------------
 * Utility functions
 *--------------------------------------------------------------------------- */

static long long ustime(void) {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}

static long long mstime(void) {
    return ustime()/1000;
}

static void cliRefreshPrompt(void) {
    int len;

    if (config.eval_ldb) return;
    if (config.hostsocket != NULL)
        len = snprintf(config.prompt,sizeof(config.prompt),"redis %s",
                       config.hostsocket);
    else
        len = anetFormatAddr(config.prompt, sizeof(config.prompt),
                           config.hostip, config.hostport);
    /* Add [dbnum] if needed */
    if (config.dbnum != 0)
        len += snprintf(config.prompt+len,sizeof(config.prompt)-len,"[%d]",
            config.dbnum);
    snprintf(config.prompt+len,sizeof(config.prompt)-len,"> ");
}

/*------------------------------------------------------------------------------
 * Help functions
 *--------------------------------------------------------------------------- */
static sds cliVersion(void) {
	return "redis-3.2.11";
}



/*------------------------------------------------------------------------------
 * Networking / parsing
 *--------------------------------------------------------------------------- */
static int cliConnect(int force) {
    if (context == NULL || force) {
        if (context != NULL) {
            redisFree(context);
        }

        if (config.hostsocket == NULL) {
            context = redisConnect(config.hostip,config.hostport);
        } else {
            //context = redisConnectUnix(config.hostsocket);
        }

        if (context->err) {
            fprintf(stderr,"Could not connect to Redis at ");
            if (config.hostsocket == NULL)
                fprintf(stderr,"%s:%d: %s\n",config.hostip,config.hostport,context->errstr);
            else
                fprintf(stderr,"%s: %s\n",config.hostsocket,context->errstr);
            redisFree(context);
            context = NULL;
            return REDIS_ERR;
        }

        anetKeepAlive(NULL, context->fd, REDIS_CLI_KEEPALIVE_INTERVAL);

        //if (cliAuth() != REDIS_OK)
        //    return REDIS_ERR;
        //if (cliSelect() != REDIS_OK)
        //    return REDIS_ERR;
    }
    return REDIS_OK;
}//end of cliConnect()


static sds cliFormatReplyTTY(redisReply *r, char *prefix) {
    sds out = sdsempty();
    switch (r->type) {
    case REDIS_REPLY_ERROR:
        out = sdscatprintf(out,"(error) %s\n", r->str);
    break;
    case REDIS_REPLY_STATUS:
        out = sdscat(out,r->str);
        out = sdscat(out,"\n");
    break;
    case REDIS_REPLY_INTEGER:
        out = sdscatprintf(out,"(integer) %lld\n",r->integer);
    break;
    case REDIS_REPLY_STRING:
        /* If you are producing output for the standard output we want
        * a more interesting output with quoted characters and so forth */
        out = sdscatrepr(out,r->str,r->len);
        out = sdscat(out,"\n");
    break;
    case REDIS_REPLY_NIL:
        out = sdscat(out,"(nil)\n");
    break;
    case REDIS_REPLY_ARRAY:
        if (r->elements == 0) {
            out = sdscat(out,"(empty list or set)\n");
        } else {
            unsigned int i, idxlen = 0;
            char _prefixlen[16];
            char _prefixfmt[16];
            sds _prefix;
            sds tmp;

            /* Calculate chars needed to represent the largest index */
            i = r->elements;
            do {
                idxlen++;
                i /= 10;
            } while(i);

            /* Prefix for nested multi bulks should grow with idxlen+2 spaces */
            memset(_prefixlen,' ',idxlen+2);
            _prefixlen[idxlen+2] = '\0';
            _prefix = sdscat(sdsnew(prefix),_prefixlen);

            /* Setup prefix format for every entry */
            snprintf(_prefixfmt,sizeof(_prefixfmt),"%%s%%%ud) ",idxlen);

            for (i = 0; i < r->elements; i++) {
                /* Don't use the prefix for the first element, as the parent
                 * caller already prepended the index number. */
                out = sdscatprintf(out,_prefixfmt,i == 0 ? "" : prefix,i+1);

                /* Format the multi bulk entry */
                tmp = cliFormatReplyTTY(r->element[i],_prefix);
                out = sdscatlen(out,tmp,sdslen(tmp));
                sdsfree(tmp);
            }
            sdsfree(_prefix);
        }
    break;
    default:
        fprintf(stderr,"Unknown reply type: %d\n", r->type);
        exit(1);
    }
    return out;
}



/* Helpe  function for sdsCatColorizedLdbReply() appending colorize strings
 * to an SDS string. */
sds sdscatcolor(sds o, char *s, size_t len, char *color) {
	return sdscatlen(o,s,len);
}




/* Colorize Lua debugger status replies according to the prefix they
 * have. */
sds sdsCatColorizedLdbReply(sds o, char *s, size_t len) {
    char *color = "white";

    if (strstr(s,"<debug>")) color = "bold";
    if (strstr(s,"<redis>")) color = "green";
    if (strstr(s,"<reply>")) color = "cyan";
    if (strstr(s,"<error>")) color = "red";
    if (strstr(s,"<hint>")) color = "bold";
    if (strstr(s,"<value>") || strstr(s,"<retval>")) color = "magenta";
    if (len > 4 && isdigit(s[3])) {
        if (s[1] == '>') color = "yellow"; /* Current line. */
        else if (s[2] == '#') color = "bold"; /* Break point. */
    }
    return sdscatcolor(o,s,len,color);
}

static sds cliFormatReplyRaw(redisReply *r) {
    sds out = sdsempty(), tmp;
    size_t i;

    switch (r->type) {
    case REDIS_REPLY_NIL:
        /* Nothing... */
        break;
    case REDIS_REPLY_ERROR:
        out = sdscatlen(out,r->str,r->len);
        out = sdscatlen(out,"\n",1);
        break;
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_STRING:
        if (r->type == REDIS_REPLY_STATUS && config.eval_ldb) {
            /* The Lua debugger replies with arrays of simple (status)
             * strings. We colorize the output for more fun if this
             * is a debugging session. */

            /* Detect the end of a debugging session. */
            if (strstr(r->str,"<endsession>") == r->str) {
                config.enable_ldb_on_eval = 0;
                config.eval_ldb = 0;
                config.eval_ldb_end = 1; /* Signal the caller session ended. */
                config.output = OUTPUT_STANDARD;
                cliRefreshPrompt();
            } else {
                out = sdsCatColorizedLdbReply(out,r->str,r->len);
            }
        } else {
            out = sdscatlen(out,r->str,r->len);
        }
        break;
    case REDIS_REPLY_INTEGER:
        out = sdscatprintf(out,"%lld",r->integer);
        break;
    case REDIS_REPLY_ARRAY:
        for (i = 0; i < r->elements; i++) {
            if (i > 0) out = sdscat(out,config.mb_delim);
            tmp = cliFormatReplyRaw(r->element[i]);
            out = sdscatlen(out,tmp,sdslen(tmp));
            sdsfree(tmp);
        }
        break;
    default:
        fprintf(stderr,"Unknown reply type: %d\n", r->type);
        exit(1);
    }
    return out;
}

static int cliReadReply(int output_raw_strings) {
    void *_reply;
    redisReply *reply;
    sds out = NULL;
    int output = 1;

    if (redisGetReply(context,&_reply) != REDIS_OK) {
        //if (config.shutdown) {
        //    redisFree(context);
        //    context = NULL;
        //    return REDIS_OK;
        //}
        if (config.interactive) {
            if (context->err == REDIS_ERR_IO &&
                (errno == ECONNRESET || errno == EPIPE))
                return REDIS_ERR;
            if (context->err == REDIS_ERR_EOF)
                return REDIS_ERR;
        }
        //cliPrintContextError();
        exit(1);
        return REDIS_ERR; /* avoid compiler warning */
    }

    reply = (redisReply*)_reply;

    config.last_cmd_type = reply->type;

    if (output) {
        if (output_raw_strings) {
            out = cliFormatReplyRaw(reply);
        } else {
            if (config.output == OUTPUT_RAW) {
                //out = cliFormatReplyRaw(reply);
                //out = sdscat(out,"\n");
            } else if (config.output == OUTPUT_STANDARD) {
                out = cliFormatReplyTTY(reply,"");
            } else if (config.output == OUTPUT_CSV) {
                //out = cliFormatReplyCSV(reply);
                //out = sdscat(out,"\n");
            }
        }
        fwrite(out,sdslen(out),1,stdout);
        sdsfree(out);
    }
	//freeReplyObject(reply);
    return REDIS_OK;
}//end of cliReadReply(int output_raw_strings)

static int cliSendCommand(int argc, char **argv, int repeat) {
    char *command = argv[0];
    size_t *argvlen;
    int j, output_raw;

    if (context == NULL) return REDIS_ERR;

    output_raw = 0;

    argvlen = zmalloc(argc*sizeof(size_t));
    for (j = 0; j < argc; j++)
        argvlen[j] = sdslen(argv[j]);

    while(repeat--) {
        redisAppendCommandArgv(context,argc,(const char**)argv,argvlen);

        if (cliReadReply(output_raw) != REDIS_OK) {
            zfree(argvlen);
            return REDIS_ERR;
        } else {
            ///* Store database number when SELECT was successfully executed. */
            //if (!strcasecmp(command,"select") && argc == 2 && config.last_cmd_type != REDIS_REPLY_ERROR) {
            //    config.dbnum = atoi(argv[1]);
            //    cliRefreshPrompt();
            //} else if (!strcasecmp(command,"auth") && argc == 2) {
            //    cliSelect();
            //}
        }
        if (config.interval) usleep(config.interval);
        fflush(stdout); /* Make it grep friendly */
    }

    zfree(argvlen);
    return REDIS_OK;
}

/*------------------------------------------------------------------------------
 * User interface
 *--------------------------------------------------------------------------- */
static int parseOptions(int argc, char **argv) {
    int i;

    for (i = 1; i < argc; i++) {
        int lastarg = i==argc-1;

        if (!strcmp(argv[i],"-h") && !lastarg) {
            sdsfree(config.hostip);
            config.hostip = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"-h") && lastarg) {
            usage();
        } else if (!strcmp(argv[i],"--help")) {
            usage();
        } else if (!strcmp(argv[i],"-x")) {
            config.stdinarg = 1;
        } else if (!strcmp(argv[i],"-p") && !lastarg) {
            config.hostport = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"-s") && !lastarg) {
            config.hostsocket = argv[++i];
        } else if (!strcmp(argv[i],"-r") && !lastarg) {
            config.repeat = strtoll(argv[++i],NULL,10);
        } else if (!strcmp(argv[i],"-i") && !lastarg) {
            double seconds = atof(argv[++i]);
            config.interval = seconds*1000000;
        } else if (!strcmp(argv[i],"-n") && !lastarg) {
            config.dbnum = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"-a") && !lastarg) {
            config.auth = argv[++i];
        } else if (!strcmp(argv[i],"--raw")) {
            config.output = OUTPUT_RAW;
        } else if (!strcmp(argv[i],"--no-raw")) {
            config.output = OUTPUT_STANDARD;
        } else if (!strcmp(argv[i],"--csv")) {
            config.output = OUTPUT_CSV;
        } else if (!strcmp(argv[i],"--latency")) {
            config.latency_mode = 1;
        } else if (!strcmp(argv[i],"--latency-dist")) {
            config.latency_dist_mode = 1;
        } else if (!strcmp(argv[i],"--mono")) {
            spectrum_palette = spectrum_palette_mono;
            spectrum_palette_size = spectrum_palette_mono_size;
        } else if (!strcmp(argv[i],"--latency-history")) {
            config.latency_mode = 1;
            config.latency_history = 1;
        } else if (!strcmp(argv[i],"--lru-test") && !lastarg) {
            config.lru_test_mode = 1;
            config.lru_test_sample_size = strtoll(argv[++i],NULL,10);
        } else if (!strcmp(argv[i],"--slave")) {
            config.slave_mode = 1;
        } else if (!strcmp(argv[i],"--stat")) {
            config.stat_mode = 1;
        } else if (!strcmp(argv[i],"--scan")) {
            config.scan_mode = 1;
        } else if (!strcmp(argv[i],"--pattern") && !lastarg) {
            config.pattern = argv[++i];
        } else if (!strcmp(argv[i],"--intrinsic-latency") && !lastarg) {
            config.intrinsic_latency_mode = 1;
            config.intrinsic_latency_duration = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--rdb") && !lastarg) {
            config.getrdb_mode = 1;
            config.rdb_filename = argv[++i];
        } else if (!strcmp(argv[i],"--pipe")) {
            config.pipe_mode = 1;
        } else if (!strcmp(argv[i],"--pipe-timeout") && !lastarg) {
            config.pipe_timeout = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--bigkeys")) {
            config.bigkeys = 1;
        } else if (!strcmp(argv[i],"--eval") && !lastarg) {
            config.eval = argv[++i];
        } else if (!strcmp(argv[i],"--ldb")) {
            config.eval_ldb = 1;
            config.output = OUTPUT_RAW;
        } else if (!strcmp(argv[i],"--ldb-sync-mode")) {
            config.eval_ldb = 1;
            config.eval_ldb_sync = 1;
            config.output = OUTPUT_RAW;
        } else if (!strcmp(argv[i],"-c")) {
            config.cluster_mode = 1;
        } else if (!strcmp(argv[i],"-d") && !lastarg) {
            sdsfree(config.mb_delim);
            config.mb_delim = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"-v") || !strcmp(argv[i], "--version")) {
            sds version = cliVersion();
            printf("redis-cli %s\n", version);
            sdsfree(version);
            exit(0);
        } else {
            if (argv[i][0] == '-') {
                fprintf(stderr,
                    "Unrecognized option or bad number of args for: '%s'\n",
                    argv[i]);
                exit(1);
            } else {
                /* Likely the command name, stop here. */
                break;
            }
        }
    }

    /* --ldb requires --eval. */
    if (config.eval_ldb && config.eval == NULL) {
        fprintf(stderr,"Options --ldb and --ldb-sync-mode require --eval.\n");
        fprintf(stderr,"Try %s --help for more information.\n", argv[0]);
        exit(1);
    }
    return i;
}//end of parseOptions()

static void usage(void) {
    sds version = cliVersion();
    fprintf(stderr,
"redis-cli %s\n"
"\n"
"Usage: redis-cli [OPTIONS] [cmd [arg [arg ...]]]\n"
"  -h <hostname>      Server hostname (default: 127.0.0.1).\n"
"  -p <port>          Server port (default: 6379).\n"
"  -s <socket>        Server socket (overrides hostname and port).\n"
"  -a <password>      Password to use when connecting to the server.\n"
"  -r <repeat>        Execute specified command N times.\n"
"  -i <interval>      When -r is used, waits <interval> seconds per command.\n"
"                     It is possible to specify sub-second times like -i 0.1.\n"
"  -n <db>            Database number.\n"
"  -x                 Read last argument from STDIN.\n"
"  -d <delimiter>     Multi-bulk delimiter in for raw formatting (default: \\n).\n"
"  -c                 Enable cluster mode (follow -ASK and -MOVED redirections).\n"
"  --raw              Use raw formatting for replies (default when STDOUT is\n"
"                     not a tty).\n"
"  --no-raw           Force formatted output even when STDOUT is not a tty.\n"
"  --csv              Output in CSV format.\n"
"  --stat             Print rolling stats about server: mem, clients, ...\n"
"  --latency          Enter a special mode continuously sampling latency.\n"
"  --latency-history  Like --latency but tracking latency changes over time.\n"
"                     Default time interval is 15 sec. Change it using -i.\n"
"  --latency-dist     Shows latency as a spectrum, requires xterm 256 colors.\n"
"                     Default time interval is 1 sec. Change it using -i.\n"
"  --lru-test <keys>  Simulate a cache workload with an 80-20 distribution.\n"
"  --slave            Simulate a slave showing commands received from the master.\n"
"  --rdb <filename>   Transfer an RDB dump from remote server to local file.\n"
"  --pipe             Transfer raw Redis protocol from stdin to server.\n"
"  --pipe-timeout <n> In --pipe mode, abort with error if after sending all data.\n"
"                     no reply is received within <n> seconds.\n"
"                     Default timeout: %d. Use 0 to wait forever.\n"
"  --bigkeys          Sample Redis keys looking for big keys.\n"
"  --scan             List all keys using the SCAN command.\n"
"  --pattern <pat>    Useful with --scan to specify a SCAN pattern.\n"
"  --intrinsic-latency <sec> Run a test to measure intrinsic system latency.\n"
"                     The test will run for the specified amount of seconds.\n"
"  --eval <file>      Send an EVAL command using the Lua script at <file>.\n"
"  --ldb              Used with --eval enable the Redis Lua debugger.\n"
"  --ldb-sync-mode    Like --ldb but uses the synchronous Lua debugger, in\n"
"                     this mode the server is blocked and script changes are\n"
"                     are not rolled back from the server memory.\n"
"  --help             Output this help and exit.\n"
"  --version          Output version and exit.\n"
"\n"
"Examples:\n"
"  cat /etc/passwd | redis-cli -x set mypasswd\n"
"  redis-cli get mypasswd\n"
"  redis-cli -r 100 lpush mylist x\n"
"  redis-cli -r 100 -i 1 info | grep used_memory_human:\n"
"  redis-cli --eval myscript.lua key1 key2 , arg1 arg2 arg3\n"
"  redis-cli --scan --pattern '*:12345*'\n"
"\n"
"  (Note: when using --eval the comma separates KEYS[] from ARGV[] items)\n"
"\n"
"When no command is given, redis-cli starts in interactive mode.\n"
"Type \"help\" in interactive mode for information on available commands\n"
"and settings.\n"
"\n",
        version, REDIS_CLI_DEFAULT_PIPE_TIMEOUT);
    sdsfree(version);
    exit(1);
}//end of usage()

static int issueCommandRepeat(int argc, char **argv, long repeat) {
	while (1) {
		config.cluster_reissue_command = 0;
		if (cliSendCommand(argc,argv,repeat) != REDIS_OK) {
			cliConnect(1);

			/* If we still cannot send the command print error.
			 * We'll try to reconnect the next time. */
			if (cliSendCommand(argc,argv,repeat) != REDIS_OK) {
				printf("cliSendCommand() error\n");
				//cliPrintContextError();
				return REDIS_ERR;
			}
		}
		/* Issue the command again if we got redirected in cluster mode */
		if (config.cluster_mode && config.cluster_reissue_command) {
			cliConnect(1);
		} else {
			break;
		}
	}
	return REDIS_OK;
}//end of issueCommandRepeat()

#define LINENOISE_MAX_LINE 4096
char * mylinenoise(char *prompt)
{
	int ret;
	char buf[LINENOISE_MAX_LINE];
	printf("%s",prompt);
	write(STDOUT_FILENO, prompt, strlen(prompt));
	ret = read(STDIN_FILENO, buf, LINENOISE_MAX_LINE);
	if (ret<=0) {
		printf("read error\n");
		exit(1);
	}else{
		return buf;
	}
}

static sds *cliSplitArgs(char *line, int *argc) {
	return sdssplitargs(line,argc);
}

static void repl(void) {
	sds historyfile = NULL;
	int history = 0;
	char *line;
	int argc;
	sds *argv;

	config.interactive = 1;

	cliRefreshPrompt();

	while((line = mylinenoise(context ? config.prompt : "not connected> ")) != NULL) {
		if (line[0] != '\0') {
			argv = cliSplitArgs(line,&argc);
			int i;
			//for (i = 0; i < argc; i++) {
			//	printf("%s\n",argv[i]);
			//}
			if (argv == NULL) {
				printf("Invalid argument(s)\n");
				continue;
			} else if (argc > 0) {
				if (strcasecmp(argv[0],"quit") == 0 ||
						strcasecmp(argv[0],"exit") == 0)
				{
					exit(0);
				} else
				{
					long long start_time = mstime(), elapsed;
					int repeat, skipargs = 0;
					char *endptr;

					repeat = strtol(argv[0], &endptr, 10);
					if (argc > 1 && *endptr == '\0' && repeat) {
						skipargs = 1;
					} else {
						repeat = 1;
					}

					issueCommandRepeat(argc-skipargs, argv+skipargs, repeat);

					elapsed = mstime()-start_time;
					if (elapsed >= 500 &&
							config.output == OUTPUT_STANDARD)
					{
						printf("(%.2fs)\n",(double)elapsed/1000);
					}
				}
			}
		}
		/* linenoise() returns malloc-ed lines like readline() */
		//linenoiseFree(line);
	}

	exit(0);
}//end of repl(void)

int main(int argc, char *argv[])
{
	int firstarg;

	config.hostip = sdsnew("127.0.0.1");
	config.hostport = 6379;
	config.hostsocket = NULL;
	config.repeat = 1;
	config.interval = 0;
	config.dbnum = 0;
	config.interactive = 0;
	config.shutdown = 0;
	config.monitor_mode = 0;
	config.pubsub_mode = 0;
	config.latency_mode = 0;
	config.latency_dist_mode = 0;
	config.latency_history = 0;
	config.lru_test_mode = 0;
	config.lru_test_sample_size = 0;
	config.cluster_mode = 0;
	config.slave_mode = 0;
	config.getrdb_mode = 0;
	config.stat_mode = 0;
	config.scan_mode = 0;
	config.intrinsic_latency_mode = 0;
	config.pattern = NULL;
	config.rdb_filename = NULL;
	config.pipe_mode = 0;
	config.pipe_timeout = REDIS_CLI_DEFAULT_PIPE_TIMEOUT;
	config.bigkeys = 0;
	config.stdinarg = 0;
	config.auth = NULL;
	config.eval = NULL;
	config.eval_ldb = 0;
	config.eval_ldb_end = 0;
	config.eval_ldb_sync = 0;
	config.enable_ldb_on_eval = 0;
	config.last_cmd_type = -1;

	pref.hints = 1;

	spectrum_palette = spectrum_palette_color;
	spectrum_palette_size = spectrum_palette_color_size;

	if (!isatty(fileno(stdout)) && (getenv("FAKETTY") == NULL))
		config.output = OUTPUT_RAW;
	else
		config.output = OUTPUT_STANDARD;
	config.mb_delim = sdsnew("\n");

	firstarg = parseOptions(argc,argv);
	argc -= firstarg;
	argv += firstarg;

	if (argc == 0 && !config.eval) {
		signal(SIGPIPE, SIG_IGN);

		cliConnect(0);
		repl();
	}
	return 0;
}
