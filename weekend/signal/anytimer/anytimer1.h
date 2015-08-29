#ifndef ANYTIMER_H__
#define ANYTIMER_H__

#define JOB_MAX		1024

typedef void at_jobfunc_t(void *);

int at_addjob(int sec, at_jobfunc_t *jobp,void *arg);
/*
 *	return	>= 0			成功，返回任务ID
 *			== -ENOMEM		失败，内存分配失败
 *			== -EINVAL		失败，参数非法
 *			== -ENOSPC		失败，任务数组满
 * */

int at_canceljob(int id);
/*	
 *	return	== 0			成功，指定任务已取消	
 *			== -EINVAL      失败，参数非法
 *			== -EBUSY		失败，指定任务已完成	
 *			== -ECANCELED	失败，指定任务是取消态
 *	*/

int at_waitjob(int id);
/*
 *	return	== 0			成功，指定任务已释放
 *			== -EINVAL      失败，参数非法
 * */


#endif


