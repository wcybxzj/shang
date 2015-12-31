#ifndef SEQ_STACK_H
#define SEQ_STACK_H

#define STACKSIZE 20

typedef enum{OK, FAIL}status_t;
typedef int datatype_t;

typedef struct stack_st {
	datatype_t arr[STACKSIZE];
	int top;
}STACK;

STACK *initstack(void);

status_t emptystack(STACK *);

status_t fullstack(STACK *);

status_t pushstack(STACK *, datatype_t );

status_t popstack(STACK *, datatype_t *);

void destroystack(STACK *);

#endif
