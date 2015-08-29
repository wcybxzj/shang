#ifndef MYTBF_H__
#define MYTBF_H__

typedef void mytbf_t;

#define MYTBF_MAX		1024

mytbf_t *mytbf_init(int cps,int burst);

int mytbf_fetchtoken(mytbf_t *,int size);

int mytbf_returntoken(mytbf_t *,int size);

int mytbf_destroy(mytbf_t *);


#endif







