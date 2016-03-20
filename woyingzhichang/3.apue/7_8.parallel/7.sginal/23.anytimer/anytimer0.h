#ifndef ANYTIMER_H__
#define ANYTIMER_H__

#define JOB_MAX		1024

typedef void at_jobfunc_t(void *);

int at_addjob(int sec,at_jobfunc_t *jobp,void *arg);
/*
 *	return:	>= 0			成功，返回指定任务ID
 *			== -EINVAL		失败，参数非法
 *			== -ENOMEM		失败，内存分存失败
 *			== -ENOSPC		失败，任务数组已满	
 * */



#endif


