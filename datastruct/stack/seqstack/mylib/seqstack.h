#ifndef SEQ_STACK_H
#define SEQ_STACK_H

#define STACKSIZE 50
#define STACKINCR 10

typedef enum{OK, FAIL}status_t;


typedef void STACK;

STACK *initstack(int size);

status_t emptystack(STACK *);

status_t fullstack(STACK *);

status_t pushstack(STACK *, datatype_t );

status_t popstack(STACK *, datatype_t *);

void destroystack(STACK *);

#endif
