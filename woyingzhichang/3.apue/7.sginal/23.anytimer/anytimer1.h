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

int at_canceljob(int id);
/*
 *	return:	== 0			成功，指定任务已取消
 *			== -EINVAL      失败，参数非法
 *			== -ECANCELED	失败，指定任务早已被取消
 *			== -EBUSY		失败，指定任务已完成			
 * */

int at_waitjob(int id);
/*
 *	return:	== 0			成功，指定任务已释放
 *			== -EINVAL      失败，参数非法
 * */

/***************************************/

//	int at_stopjob(int id);
//	int at_resumejob(int id);


#endif


