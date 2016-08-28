#ifndef MYPIPE
#define MYPIPE

#define PIPESIZE 1024

#define MYPIPE_READ 0x00000001U
#define MYPIPE_WRITE 0x00000002U

#define MYPIPE_INIT_FAIL ((void *)-1)
#define Q_CREATE_FAIL ((void *)-2)


typedef void mypipe_t;

mypipe_t *mypipe_init();

int mypipe_register(mypipe_t *, int opmap);

int mypipe_unregister(mypipe_t *, int opmap);

ssize_t mypipe_read(mypipe_t *, void *buf, size_t count);

ssize_t mypipe_write(mypipe_t *, const void *buf, size_t count);

void mypipe_destroy(mypipe_t *);

#endif
