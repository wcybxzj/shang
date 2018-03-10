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
#include "rdb.h"

#define MAXPATHLEN 100

int ybx_rdbSaveRio(rio *rdb, int *error) {
    dictIterator *di = NULL;
    dictEntry *de;
    char magic[10];
    int j;
    long long now = mstime();
    uint64_t cksum;

    if (server.rdb_checksum)
        rdb->update_cksum = rioGenericUpdateChecksum;
    snprintf(magic,sizeof(magic),"REDIS%04d",RDB_VERSION);
    if (rdbWriteRaw(rdb,magic,9) == -1) goto werr;
    if (rdbSaveInfoAuxFields(rdb) == -1) goto werr;

    //for (j = 0; j < server.dbnum; j++) {
    //    redisDb *db = server.db+j;
    //    dict *d = db->dict;
    //    if (dictSize(d) == 0) continue;
    //    di = dictGetSafeIterator(d);
    //    if (!di) return C_ERR;

    //    /* Write the SELECT DB opcode */
    //    if (rdbSaveType(rdb,RDB_OPCODE_SELECTDB) == -1) goto werr;
    //    if (rdbSaveLen(rdb,j) == -1) goto werr;

    //    /* Write the RESIZE DB opcode. We trim the size to UINT32_MAX, which
    //     * is currently the largest type we are able to represent in RDB sizes.
    //     * However this does not limit the actual size of the DB to load since
    //     * these sizes are just hints to resize the hash tables. */
    //    uint32_t db_size, expires_size;
    //    db_size = (dictSize(db->dict) <= UINT32_MAX) ?
    //                            dictSize(db->dict) :
    //                            UINT32_MAX;
    //    expires_size = (dictSize(db->expires) <= UINT32_MAX) ?
    //                            dictSize(db->expires) :
    //                            UINT32_MAX;
    //    if (rdbSaveType(rdb,RDB_OPCODE_RESIZEDB) == -1) goto werr;
    //    if (rdbSaveLen(rdb,db_size) == -1) goto werr;
    //    if (rdbSaveLen(rdb,expires_size) == -1) goto werr;

    //    /* Iterate this DB writing every entry */
    //    while((de = dictNext(di)) != NULL) {
    //        sds keystr = dictGetKey(de);
    //        robj key, *o = dictGetVal(de);
    //        long long expire;

    //        initStaticStringObject(key,keystr);
    //        expire = getExpire(db,&key);
    //        if (rdbSaveKeyValuePair(rdb,&key,o,expire,now) == -1) goto werr;
    //    }
    //    dictReleaseIterator(di);
    //}
    //di = NULL; /* So that we don't release it again on error. */

    ///* EOF opcode */
    //if (rdbSaveType(rdb,RDB_OPCODE_EOF) == -1) goto werr;

    ///* CRC64 checksum. It will be zero if checksum computation is disabled, the
    // * loading code skips the check in this case. */
    //cksum = rdb->cksum;
    //memrev64ifbe(&cksum);
    //if (rioWrite(rdb,&cksum,8) == 0) goto werr;
    return C_OK;

werr:
    //if (error) *error = errno;
    //if (di) dictReleaseIterator(di);
    return C_ERR;
}

int ybx_rdbSave(char *filename) {
    char tmpfile[256];
    char cwd[MAXPATHLEN];
    FILE *fp;
    rio rdb;
    int error = 0;

    snprintf(tmpfile,256,"temp-%d.rdb", (int) getpid());
    fp = fopen(tmpfile,"w");
    if (!fp) {
        char *cwdp = getcwd(cwd,MAXPATHLEN);
        return C_ERR;
    }

    rioInitWithFile(&rdb,fp);
    if (ybx_rdbSaveRio(&rdb,&error) == C_ERR) {
        errno = error;
        goto werr;
    }

    if (fflush(fp) == EOF) goto werr;
    if (fsync(fileno(fp)) == -1) goto werr;
    if (fclose(fp) == EOF) goto werr;

    if (rename(tmpfile,filename) == -1) {
        char *cwdp = getcwd(cwd,MAXPATHLEN);
        unlink(tmpfile);
        return C_ERR;
    }


    server.dirty = 0;
    server.lastsave = time(NULL);
    server.lastbgsave_status = C_OK;
    return C_OK;

werr:
    fclose(fp);
    unlink(tmpfile);
    return C_ERR;
}



int main(int argc, const char *argv[])
{
	ybx_rdbSave("ybxdump.rdb");
	return 0;
}
