#include "fmacros.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "rio.h"
#include "util.h"
#include "crc64.h"
#include "config.h"
#include "server.h"

/* ------------------------- Buffer I/O implementation ----------------------- */


/* --------------------- Stdio file pointer implementation ------------------- */

/* Returns 1 or 0 for success/failure. */
static size_t rioFileWrite(rio *r, const void *buf, size_t len) {
    size_t retval;

    retval = fwrite(buf,len,1,r->io.file.fp);
    r->io.file.buffered += len;

    if (r->io.file.autosync &&
        r->io.file.buffered >= r->io.file.autosync)
    {
        fflush(r->io.file.fp);
        aof_fsync(fileno(r->io.file.fp));
        r->io.file.buffered = 0;
    }
    return retval;
}

/* Returns 1 or 0 for success/failure. */
static size_t rioFileRead(rio *r, void *buf, size_t len) {
    return fread(buf,len,1,r->io.file.fp);
}

/* Returns read/write position in buffer. */
static off_t rioBufferTell(rio *r) {
    return r->io.buffer.pos;
}

/* Flushes any buffer to target device if applicable. Returns 1 on success
 * and 0 on failures. */
static int rioBufferFlush(rio *r) {
    UNUSED(r);
    return 1; /* Nothing to do, our write just appends to the buffer. */
}

/* Returns read/write position in file. */
static off_t rioFileTell(rio *r) {
    return ftello(r->io.file.fp);
}

/* Flushes any buffer to target device if applicable. Returns 1 on success
 * and 0 on failures. */
static int rioFileFlush(rio *r) {
    return (fflush(r->io.file.fp) == 0) ? 1 : 0;
}

static const rio rioFileIO = {
	rioFileRead,
	rioFileWrite,
	rioFileTell,
	rioFileFlush,
	NULL,           /* update_checksum */
	0,              /* current checksum */
	0,              /* bytes read or written */
	0,              /* read/write chunk size */
	{ { NULL, 0 } } /* union for io-specific vars */
};

void rioInitWithFile(rio *r, FILE *fp) {
	*r = rioFileIO;
	r->io.file.fp = fp;
	r->io.file.buffered = 0;
	r->io.file.autosync = 0;
}

/* ------------------- File descriptors set implementation ------------------- */

/* ---------------------------- Generic functions ---------------------------- */
/* This function can be installed both in memory and file streams when checksum
 * computation is needed. */
void rioGenericUpdateChecksum(rio *r, const void *buf, size_t len) {
    r->cksum = crc64(r->cksum,buf,len);
}

// 设置自动同步的字节数限制，如果bytes为0，则意味着不执行
void rioSetAutoSync(rio *r, off_t bytes) {
    serverAssert(r->read == rioFileIO.read);    //限制为文件流对象，不对其他对象设置限制
    r->io.file.autosync = bytes;
}

/* --------------------------- Higher level interface --------------------------
 *
 * The following higher level functions use lower level rio.c functions to help
 * generating the Redis protocol for the Append Only File. */
// 下面的高级函数调用上面的低级函数来辅助生成AOF文件的协议

/* Write multi bulk count in the format: "*<count>\r\n". */
// 以"*<count>\r\n"格式为写如一个int整型的count
size_t rioWriteBulkCount(rio *r, char prefix, int count) {
    char cbuf[128];
    int clen;

    // 构建一个 "*<count>\r\n"
    cbuf[0] = prefix;
    clen = 1+ll2string(cbuf+1,sizeof(cbuf)-1,count);
    cbuf[clen++] = '\r';
    cbuf[clen++] = '\n';
    // 调用rio的接口，将cbuf写如r中
    if (rioWrite(r,cbuf,clen) == 0) return 0;
    return clen;
}

/* Write binary-safe string in the format: "$<count>\r\n<payload>\r\n". */
// 以"$<count>\r\n<payload>\r\n"为格式写入一个字符串
size_t rioWriteBulkString(rio *r, const char *buf, size_t len) {
    size_t nwritten;

    // 写入"$<len>\r\n"
    if ((nwritten = rioWriteBulkCount(r,'$',len)) == 0) return 0;
    // 追加写入一个buf，也就是<payload>部分
    if (len > 0 && rioWrite(r,buf,len) == 0) return 0;
    // 追加"\r\n"
    if (rioWrite(r,"\r\n",2) == 0) return 0;
    return nwritten+len+2;  //返回长度
}

/* Write a long long value in format: "$<count>\r\n<payload>\r\n". */
// 以"$<count>\r\n<payload>\r\n"为格式写入一个longlong 值
size_t rioWriteBulkLongLong(rio *r, long long l) {
    char lbuf[32];
    unsigned int llen;

    // 将longlong转为字符串，按字符串的格式写入
    llen = ll2string(lbuf,sizeof(lbuf),l);
    return rioWriteBulkString(r,lbuf,llen);
}

/* Write a double value in the format: "$<count>\r\n<payload>\r\n" */
// 以"$<count>\r\n<payload>\r\n"为格式写入一个 double 值
size_t rioWriteBulkDouble(rio *r, double d) {
    char dbuf[128];
    unsigned int dlen;

    //以宽度为17位的方式写到dbuf中，17位的double双精度浮点数的长度最短且无损
    dlen = snprintf(dbuf,sizeof(dbuf),"%.17g",d);
    return rioWriteBulkString(r,dbuf,dlen);
}



