#include "server.h"
#include "cluster.h"

#include <fcntl.h>
#include <sys/stat.h>

/*-----------------------------------------------------------------------------
 * Config file name-value maps.
 *----------------------------------------------------------------------------*/

typedef struct configEnum {
    const char *name;
    const int val;
} configEnum;

configEnum maxmemory_policy_enum[] = {
    {"volatile-lru", MAXMEMORY_VOLATILE_LRU},
    {"volatile-random",MAXMEMORY_VOLATILE_RANDOM},
    {"volatile-ttl",MAXMEMORY_VOLATILE_TTL},
    {"allkeys-lru",MAXMEMORY_ALLKEYS_LRU},
    {"allkeys-random",MAXMEMORY_ALLKEYS_RANDOM},
    {"noeviction",MAXMEMORY_NO_EVICTION},
    {NULL, 0}
};

configEnum syslog_facility_enum[] = {
    {"user",    LOG_USER},
    {"local0",  LOG_LOCAL0},
    {"local1",  LOG_LOCAL1},
    {"local2",  LOG_LOCAL2},
    {"local3",  LOG_LOCAL3},
    {"local4",  LOG_LOCAL4},
    {"local5",  LOG_LOCAL5},
    {"local6",  LOG_LOCAL6},
    {"local7",  LOG_LOCAL7},
    {NULL, 0}
};

configEnum loglevel_enum[] = {
    {"debug", LL_DEBUG},
    {"verbose", LL_VERBOSE},
    {"notice", LL_NOTICE},
    {"warning", LL_WARNING},
    {NULL,0}
};

configEnum supervised_mode_enum[] = {
    {"upstart", SUPERVISED_UPSTART},
    {"systemd", SUPERVISED_SYSTEMD},
    {"auto", SUPERVISED_AUTODETECT},
    {"no", SUPERVISED_NONE},
    {NULL, 0}
};

configEnum aof_fsync_enum[] = {
    {"everysec", AOF_FSYNC_EVERYSEC},
    {"always", AOF_FSYNC_ALWAYS},
    {"no", AOF_FSYNC_NO},
    {NULL, 0}
};

/* Output buffer limits presets. */
clientBufferLimitsConfig clientBufferLimitsDefaults[CLIENT_TYPE_OBUF_COUNT] = {
    {0, 0, 0}, /* normal */
    {1024*1024*256, 1024*1024*64, 60}, /* slave */
    {1024*1024*32, 1024*1024*8, 60}  /* pubsub */
};

/*-----------------------------------------------------------------------------
 * Enum access functions
 *----------------------------------------------------------------------------*/

/* Get enum value from name. If there is no match INT_MIN is returned. */
int configEnumGetValue(configEnum *ce, char *name) {
    while(ce->name != NULL) {
        if (!strcasecmp(ce->name,name)) return ce->val;
        ce++;
    }
    return INT_MIN;
}



/*-----------------------------------------------------------------------------
 * Config file parsing
 *----------------------------------------------------------------------------*/


int yesnotoi(char *s) {
    if (!strcasecmp(s,"yes")) return 1;
    else if (!strcasecmp(s,"no")) return 0;
    else return -1;
}


void appendServerSaveParams(time_t seconds, int changes) {
    server.saveparams = zrealloc(server.saveparams,sizeof(struct saveparam)*(server.saveparamslen+1));
    server.saveparams[server.saveparamslen].seconds = seconds;
    server.saveparams[server.saveparamslen].changes = changes;
    server.saveparamslen++;
}


void resetServerSaveParams(void) {
    zfree(server.saveparams);
    server.saveparams = NULL;
    server.saveparamslen = 0;
}

void loadServerConfigFromString(char *config) {
    char *err = NULL;
    int linenum = 0, totlines, i;
    int slaveof_linenum = 0;
    sds *lines;

    lines = sdssplitlen(config,strlen(config),"\n",1,&totlines);

    for (i = 0; i < totlines; i++) {
        sds *argv;
        int argc;

        linenum = i+1;
        lines[i] = sdstrim(lines[i]," \t\r\n");

        /* Skip comments and blank lines */
        if (lines[i][0] == '#' || lines[i][0] == '\0') continue;

        /* Split into arguments */
        argv = sdssplitargs(lines[i],&argc);
        if (argv == NULL) {
            err = "Unbalanced quotes in configuration line";
            goto loaderr;
        }

        /* Skip this line if the resulting command vector is empty. */
        if (argc == 0) {
            sdsfreesplitres(argv,argc);
            continue;
        }
        sdstolower(argv[0]);

        /* Execute config directives */
        if (!strcasecmp(argv[0],"timeout") && argc == 2) {
            server.maxidletime = atoi(argv[1]);
            if (server.maxidletime < 0) {
                err = "Invalid timeout value"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"tcp-keepalive") && argc == 2) {
            server.tcpkeepalive = atoi(argv[1]);
            if (server.tcpkeepalive < 0) {
                err = "Invalid tcp-keepalive value"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"protected-mode") && argc == 2) {
            if ((server.protected_mode = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"port") && argc == 2) {
            server.port = atoi(argv[1]);
            if (server.port < 0 || server.port > 65535) {
                err = "Invalid port"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"tcp-backlog") && argc == 2) {
            server.tcp_backlog = atoi(argv[1]);
            if (server.tcp_backlog < 0) {
                err = "Invalid backlog value"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"bind") && argc >= 2) {
            int j, addresses = argc-1;

            if (addresses > CONFIG_BINDADDR_MAX) {
                err = "Too many bind addresses specified"; goto loaderr;
            }
            for (j = 0; j < addresses; j++)
                server.bindaddr[j] = zstrdup(argv[j+1]);
            server.bindaddr_count = addresses;
        } else if (!strcasecmp(argv[0],"unixsocket") && argc == 2) {
            server.unixsocket = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"unixsocketperm") && argc == 2) {
            errno = 0;
            server.unixsocketperm = (mode_t)strtol(argv[1], NULL, 8);
            if (errno || server.unixsocketperm > 0777) {
                err = "Invalid socket file permissions"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"save")) {
            if (argc == 3) {
                int seconds = atoi(argv[1]);
                int changes = atoi(argv[2]);
                if (seconds < 1 || changes < 0) {
                    err = "Invalid save parameters"; goto loaderr;
                }
                appendServerSaveParams(seconds,changes);
            } else if (argc == 2 && !strcasecmp(argv[1],"")) {
                resetServerSaveParams();
            }
        } else if (!strcasecmp(argv[0],"dir") && argc == 2) {
            if (chdir(argv[1]) == -1) {
                serverLog(LL_WARNING,"Can't chdir to '%s': %s",
                    argv[1], strerror(errno));
                exit(1);
            }
        } else if (!strcasecmp(argv[0],"loglevel") && argc == 2) {
            server.verbosity = configEnumGetValue(loglevel_enum,argv[1]);
            if (server.verbosity == INT_MIN) {
                err = "Invalid log level. "
                      "Must be one of debug, verbose, notice, warning";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"logfile") && argc == 2) {
            FILE *logfp;

            zfree(server.logfile);
            server.logfile = zstrdup(argv[1]);
            if (server.logfile[0] != '\0') {
                /* Test if we are able to open the file. The server will not
                 * be able to abort just for this problem later... */
                logfp = fopen(server.logfile,"a");
                if (logfp == NULL) {
                    err = sdscatprintf(sdsempty(),
                        "Can't open the log file: %s", strerror(errno));
                    goto loaderr;
                }
                fclose(logfp);
            }
        } else if (!strcasecmp(argv[0],"syslog-enabled") && argc == 2) {
            if ((server.syslog_enabled = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"syslog-ident") && argc == 2) {
            if (server.syslog_ident) zfree(server.syslog_ident);
            server.syslog_ident = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"syslog-facility") && argc == 2) {
            server.syslog_facility =
                configEnumGetValue(syslog_facility_enum,argv[1]);
            if (server.syslog_facility == INT_MIN) {
                err = "Invalid log facility. Must be one of USER or between LOCAL0-LOCAL7";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"databases") && argc == 2) {
            server.dbnum = atoi(argv[1]);
            if (server.dbnum < 1) {
                err = "Invalid number of databases"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"include") && argc == 2) {
            loadServerConfig(argv[1],NULL);
        } else if (!strcasecmp(argv[0],"maxclients") && argc == 2) {
            server.maxclients = atoi(argv[1]);
            if (server.maxclients < 1) {
                err = "Invalid max clients limit"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"maxmemory") && argc == 2) {
            server.maxmemory = memtoll(argv[1],NULL);
        } else if (!strcasecmp(argv[0],"maxmemory-policy") && argc == 2) {
            server.maxmemory_policy =
                configEnumGetValue(maxmemory_policy_enum,argv[1]);
            if (server.maxmemory_policy == INT_MIN) {
                err = "Invalid maxmemory policy";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"maxmemory-samples") && argc == 2) {
            server.maxmemory_samples = atoi(argv[1]);
            if (server.maxmemory_samples <= 0) {
                err = "maxmemory-samples must be 1 or greater";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"slaveof") && argc == 3) {
            slaveof_linenum = linenum;
            server.masterhost = sdsnew(argv[1]);
            server.masterport = atoi(argv[2]);
            server.repl_state = REPL_STATE_CONNECT;
        } else if (!strcasecmp(argv[0],"repl-ping-slave-period") && argc == 2) {
            server.repl_ping_slave_period = atoi(argv[1]);
            if (server.repl_ping_slave_period <= 0) {
                err = "repl-ping-slave-period must be 1 or greater";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"repl-timeout") && argc == 2) {
            server.repl_timeout = atoi(argv[1]);
            if (server.repl_timeout <= 0) {
                err = "repl-timeout must be 1 or greater";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"repl-disable-tcp-nodelay") && argc==2) {
            if ((server.repl_disable_tcp_nodelay = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"repl-diskless-sync") && argc==2) {
            if ((server.repl_diskless_sync = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"repl-diskless-sync-delay") && argc==2) {
            server.repl_diskless_sync_delay = atoi(argv[1]);
            if (server.repl_diskless_sync_delay < 0) {
                err = "repl-diskless-sync-delay can't be negative";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"repl-backlog-size") && argc == 2) {
            long long size = memtoll(argv[1],NULL);
            if (size <= 0) {
                err = "repl-backlog-size must be 1 or greater.";
                goto loaderr;
            }
            resizeReplicationBacklog(size);
        } else if (!strcasecmp(argv[0],"repl-backlog-ttl") && argc == 2) {
            server.repl_backlog_time_limit = atoi(argv[1]);
            if (server.repl_backlog_time_limit < 0) {
                err = "repl-backlog-ttl can't be negative ";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"masterauth") && argc == 2) {
            zfree(server.masterauth);
            server.masterauth = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"slave-serve-stale-data") && argc == 2) {
            if ((server.repl_serve_stale_data = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"slave-read-only") && argc == 2) {
            if ((server.repl_slave_ro = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"rdbcompression") && argc == 2) {
            if ((server.rdb_compression = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"rdbchecksum") && argc == 2) {
            if ((server.rdb_checksum = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"activerehashing") && argc == 2) {
            if ((server.activerehashing = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"daemonize") && argc == 2) {
            if ((server.daemonize = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"hz") && argc == 2) {
            server.hz = atoi(argv[1]);
            if (server.hz < CONFIG_MIN_HZ) server.hz = CONFIG_MIN_HZ;
            if (server.hz > CONFIG_MAX_HZ) server.hz = CONFIG_MAX_HZ;
        } else if (!strcasecmp(argv[0],"appendonly") && argc == 2) {
            int yes;

            if ((yes = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
            server.aof_state = yes ? AOF_ON : AOF_OFF;
        } else if (!strcasecmp(argv[0],"appendfilename") && argc == 2) {
            if (!pathIsBaseName(argv[1])) {
                err = "appendfilename can't be a path, just a filename";
                goto loaderr;
            }
            zfree(server.aof_filename);
            server.aof_filename = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"no-appendfsync-on-rewrite")
                   && argc == 2) {
            if ((server.aof_no_fsync_on_rewrite= yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"appendfsync") && argc == 2) {
            server.aof_fsync = configEnumGetValue(aof_fsync_enum,argv[1]);
            if (server.aof_fsync == INT_MIN) {
                err = "argument must be 'no', 'always' or 'everysec'";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"auto-aof-rewrite-percentage") &&
                   argc == 2)
        {
            server.aof_rewrite_perc = atoi(argv[1]);
            if (server.aof_rewrite_perc < 0) {
                err = "Invalid negative percentage for AOF auto rewrite";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"auto-aof-rewrite-min-size") &&
                   argc == 2)
        {
            server.aof_rewrite_min_size = memtoll(argv[1],NULL);
        } else if (!strcasecmp(argv[0],"aof-rewrite-incremental-fsync") &&
                   argc == 2)
        {
            if ((server.aof_rewrite_incremental_fsync =
                 yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"aof-load-truncated") && argc == 2) {
            if ((server.aof_load_truncated = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"requirepass") && argc == 2) {
            if (strlen(argv[1]) > CONFIG_AUTHPASS_MAX_LEN) {
                err = "Password is longer than CONFIG_AUTHPASS_MAX_LEN";
                goto loaderr;
            }
            server.requirepass = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"pidfile") && argc == 2) {
            zfree(server.pidfile);
            server.pidfile = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"dbfilename") && argc == 2) {
            if (!pathIsBaseName(argv[1])) {
                err = "dbfilename can't be a path, just a filename";
                goto loaderr;
            }
            zfree(server.rdb_filename);
            server.rdb_filename = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"hash-max-ziplist-entries") && argc == 2) {
            server.hash_max_ziplist_entries = memtoll(argv[1], NULL);
        } else if (!strcasecmp(argv[0],"hash-max-ziplist-value") && argc == 2) {
            server.hash_max_ziplist_value = memtoll(argv[1], NULL);
        } else if (!strcasecmp(argv[0],"list-max-ziplist-entries") && argc == 2){
            /* DEAD OPTION */
        } else if (!strcasecmp(argv[0],"list-max-ziplist-value") && argc == 2) {
            /* DEAD OPTION */
        } else if (!strcasecmp(argv[0],"list-max-ziplist-size") && argc == 2) {
            server.list_max_ziplist_size = atoi(argv[1]);
        } else if (!strcasecmp(argv[0],"list-compress-depth") && argc == 2) {
            server.list_compress_depth = atoi(argv[1]);
        } else if (!strcasecmp(argv[0],"set-max-intset-entries") && argc == 2) {
            server.set_max_intset_entries = memtoll(argv[1], NULL);
        } else if (!strcasecmp(argv[0],"zset-max-ziplist-entries") && argc == 2) {
            server.zset_max_ziplist_entries = memtoll(argv[1], NULL);
        } else if (!strcasecmp(argv[0],"zset-max-ziplist-value") && argc == 2) {
            server.zset_max_ziplist_value = memtoll(argv[1], NULL);
        } else if (!strcasecmp(argv[0],"hll-sparse-max-bytes") && argc == 2) {
            server.hll_sparse_max_bytes = memtoll(argv[1], NULL);
        } else if (!strcasecmp(argv[0],"rename-command") && argc == 3) {
            struct redisCommand *cmd = lookupCommand(argv[1]);
            int retval;

            if (!cmd) {
                err = "No such command in rename-command";
                goto loaderr;
            }

            /* If the target command name is the empty string we just
             * remove it from the command table. */
            retval = dictDelete(server.commands, argv[1]);
            serverAssert(retval == DICT_OK);

            /* Otherwise we re-add the command under a different name. */
            if (sdslen(argv[2]) != 0) {
                sds copy = sdsdup(argv[2]);

                retval = dictAdd(server.commands, copy, cmd);
                if (retval != DICT_OK) {
                    sdsfree(copy);
                    err = "Target command name already exists"; goto loaderr;
                }
            }
        } else if (!strcasecmp(argv[0],"cluster-enabled") && argc == 2) {
            if ((server.cluster_enabled = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"cluster-config-file") && argc == 2) {
            zfree(server.cluster_configfile);
            server.cluster_configfile = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"cluster-require-full-coverage") &&
                    argc == 2)
        {
            if ((server.cluster_require_full_coverage = yesnotoi(argv[1])) == -1)
            {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"cluster-node-timeout") && argc == 2) {
            server.cluster_node_timeout = strtoll(argv[1],NULL,10);
            if (server.cluster_node_timeout <= 0) {
                err = "cluster node timeout must be 1 or greater"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"cluster-migration-barrier")
                   && argc == 2)
        {
            server.cluster_migration_barrier = atoi(argv[1]);
            if (server.cluster_migration_barrier < 0) {
                err = "cluster migration barrier must zero or positive";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"cluster-slave-validity-factor")
                   && argc == 2)
        {
            server.cluster_slave_validity_factor = atoi(argv[1]);
            if (server.cluster_slave_validity_factor < 0) {
                err = "cluster slave validity factor must be zero or positive";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"lua-time-limit") && argc == 2) {
            server.lua_time_limit = strtoll(argv[1],NULL,10);
        } else if (!strcasecmp(argv[0],"slowlog-log-slower-than") &&
                   argc == 2)
        {
            server.slowlog_log_slower_than = strtoll(argv[1],NULL,10);
        } else if (!strcasecmp(argv[0],"latency-monitor-threshold") &&
                   argc == 2)
        {
            server.latency_monitor_threshold = strtoll(argv[1],NULL,10);
            if (server.latency_monitor_threshold < 0) {
                err = "The latency threshold can't be negative";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"slowlog-max-len") && argc == 2) {
            server.slowlog_max_len = strtoll(argv[1],NULL,10);
        } else if (!strcasecmp(argv[0],"client-output-buffer-limit") &&
                   argc == 5)
        {
            int class = getClientTypeByName(argv[1]);
            unsigned long long hard, soft;
            int soft_seconds;

            if (class == -1 || class == CLIENT_TYPE_MASTER) {
                err = "Unrecognized client limit class: the user specified "
                "an invalid one, or 'master' which has no buffer limits.";
                goto loaderr;
            }
            hard = memtoll(argv[2],NULL);
            soft = memtoll(argv[3],NULL);
            soft_seconds = atoi(argv[4]);
            if (soft_seconds < 0) {
                err = "Negative number of seconds in soft limit is invalid";
                goto loaderr;
            }
            server.client_obuf_limits[class].hard_limit_bytes = hard;
            server.client_obuf_limits[class].soft_limit_bytes = soft;
            server.client_obuf_limits[class].soft_limit_seconds = soft_seconds;
        } else if (!strcasecmp(argv[0],"stop-writes-on-bgsave-error") &&
                   argc == 2) {
            if ((server.stop_writes_on_bgsave_err = yesnotoi(argv[1])) == -1) {
                err = "argument must be 'yes' or 'no'"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"slave-priority") && argc == 2) {
            server.slave_priority = atoi(argv[1]);
        } else if (!strcasecmp(argv[0],"slave-announce-ip") && argc == 2) {
            zfree(server.slave_announce_ip);
            server.slave_announce_ip = zstrdup(argv[1]);
        } else if (!strcasecmp(argv[0],"slave-announce-port") && argc == 2) {
            server.slave_announce_port = atoi(argv[1]);
            if (server.slave_announce_port < 0 ||
                server.slave_announce_port > 65535)
            {
                err = "Invalid port"; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"min-slaves-to-write") && argc == 2) {
            server.repl_min_slaves_to_write = atoi(argv[1]);
            if (server.repl_min_slaves_to_write < 0) {
                err = "Invalid value for min-slaves-to-write."; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"min-slaves-max-lag") && argc == 2) {
            server.repl_min_slaves_max_lag = atoi(argv[1]);
            if (server.repl_min_slaves_max_lag < 0) {
                err = "Invalid value for min-slaves-max-lag."; goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"notify-keyspace-events") && argc == 2) {
            int flags = keyspaceEventsStringToFlags(argv[1]);

            if (flags == -1) {
                err = "Invalid event class character. Use 'g$lshzxeA'.";
                goto loaderr;
            }
            server.notify_keyspace_events = flags;
        } else if (!strcasecmp(argv[0],"supervised") && argc == 2) {
            server.supervised_mode =
                configEnumGetValue(supervised_mode_enum,argv[1]);

            if (server.supervised_mode == INT_MIN) {
                err = "Invalid option for 'supervised'. "
                    "Allowed values: 'upstart', 'systemd', 'auto', or 'no'";
                goto loaderr;
            }
        } else if (!strcasecmp(argv[0],"sentinel")) {
            ///* argc == 1 is handled by main() as we need to enter the sentinel
            // * mode ASAP. */
            //if (argc != 1) {
            //    if (!server.sentinel_mode) {
            //        err = "sentinel directive while not in sentinel mode";
            //        goto loaderr;
            //    }
            //    err = sentinelHandleConfiguration(argv+1,argc-1);
            //    if (err) goto loaderr;
            //}
        } else {
            err = "Bad directive or wrong number of arguments"; goto loaderr;
        }
        sdsfreesplitres(argv,argc);
    }

    /* Sanity checks. */
    if (server.cluster_enabled && server.masterhost) {
        linenum = slaveof_linenum;
        i = linenum-1;
        err = "slaveof directive not allowed in cluster mode";
        goto loaderr;
    }

    sdsfreesplitres(lines,totlines);
    return;

loaderr:
    fprintf(stderr, "\n*** FATAL CONFIG FILE ERROR ***\n");
    fprintf(stderr, "Reading the configuration file, at line %d\n", linenum);
    fprintf(stderr, ">>> '%s'\n", lines[i]);
    fprintf(stderr, "%s\n", err);
    exit(1);
}//end of void loadServerConfigFromString(char *config)

/* Load the server configuration from the specified filename.
 * The function appends the additional configuration directives stored
 * in the 'options' string to the config file before loading.
 *
 * Both filename and options can be NULL, in such a case are considered
 * empty. This way loadServerConfig can be used to just load a file or
 * just load a string. */
void loadServerConfig(char *filename, char *options) {
    sds config = sdsempty();
    char buf[CONFIG_MAX_LINE+1];

    /* Load the file content */
    if (filename) {
        FILE *fp;

        if (filename[0] == '-' && filename[1] == '\0') {
            fp = stdin;
        } else {
            if ((fp = fopen(filename,"r")) == NULL) {
                serverLog(LL_WARNING,
                    "Fatal error, can't open config file '%s'", filename);
                exit(1);
            }
        }
        while(fgets(buf,CONFIG_MAX_LINE+1,fp) != NULL)
            config = sdscat(config,buf);
        if (fp != stdin) fclose(fp);
    }
    /* Append the additional options */
    if (options) {
        config = sdscat(config,"\n");
        config = sdscat(config,options);
    }

	//printf("%s\n",config);
    loadServerConfigFromString(config);
    sdsfree(config);
}



