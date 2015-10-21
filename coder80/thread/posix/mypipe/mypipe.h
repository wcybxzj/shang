#ifndef MYPIPE_H__
#define MYPIPE_H__

#define PIPESIZE		1024

typedef void mypipe_t;

mypipe_t *mypipe_init();

ssize_t mypipe_read(mypipe_t *, void *buf, size_t count);

ssize_t mypipe_write(mypipe_t *,const void *buf,size_t count);

void mypipe_destroy(mypipe_t *);


#endif


