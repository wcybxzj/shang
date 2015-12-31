#ifndef SEQ_STACK_H
#define SEQ_STACK_H

#define STACKSIZE 20

typedef enum{RIGHT, ERROR}status;
typedef int datatype_t;

typedef struct stack_st {
	datatype_t arr[STACKSIZE];
	int top;
}STACK;

STACK *initstack(void);

status emptystack(STACK *);

status fullstack(STACK *);

status pushstack(STACK *, datatype_t );

status popstack(STACK *, datatype_t *);

int get_stacknum(STACK *);

void destroystack(STACK *);

#endif
