#ifndef OTHER_STACK_H
#define OTHER_STACK_H

#define BASESIZE 50
#define INCREACE 10

typedef enum{OK, FAIL}status_t;
typedef int datatype_t;

struct stack_st {
	datatype_t *base;
	datatype_t *top;
	int stacksize;
};

struct stack_st *init_seqstack(void);

status_t empty_seqstack(struct stack_st *);

status_t full_seqstack(struct stack_st *);

status_t push_seqstack(struct stack_st *, datatype_t );

status_t pop_seqstack(struct stack_st *, datatype_t *);

void destroy_seqstack(struct stack_st *);

#endif
