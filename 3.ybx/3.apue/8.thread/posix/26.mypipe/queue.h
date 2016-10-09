#ifndef QUEUE_H__
#define QUEUE_H__

#define MAXSIZE 1024

typedef int datatype;

typedef struct _QUEUE {
	datatype data[MAXSIZE];
	int head, tail;
} Q;

Q *q_create(void);

int q_isempty(Q *ptr);

int q_isfull(Q *ptr);

int q_enqueue(Q *ptr, datatype *);

int q_dequeue(Q *ptr, datatype *);

void q_travel(Q *ptr);

void q_clear(Q *ptr);

void q_destroy(Q *ptr);

#endif
