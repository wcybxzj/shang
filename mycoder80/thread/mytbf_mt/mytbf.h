#ifndef MY_TBF_H__
#define MY_TBF_H__
	
#define MYTBF_MAX 1024

typedef	void mytbf_t;


mytbf_t *mytbf_init(int cps, int burst);
int mytbf_fetchtoken(mytbf_t *, int);
int mytbf_returntokeb(mytbf_t *, int);
int mytbf_destroy(mytbf_t *);
#endif
