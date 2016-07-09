#ifndef MYSEM_H__
#define MYSEM_H__

typedef void mysem_t;

mysem_t* mysem_init(int init_num);
int mysem_add(mysem_t*ptr, int num);
int mysem_sub(mysem_t*ptr, int num);
void mysem_destory(mysem_t*ptr);

#endif
