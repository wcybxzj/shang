//线程池编程实现
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>//信号量sem_t
#include <pthread.h>


//任务接口,线程调用的函数
typedef void* (*FUNC)(void *arg);

//任务数据结构
typedef struct thread_pool_job_s{
        FUNC function;//线程调用的函数
        void *arg;//函数参数
        struct thread_pool_job_s *pre;//指向上一个节点
        struct thread_pool_job_s *next;//指向下一个节点
}thread_pool_job;

//工作队列
typedef struct thread_pool_job_queue_s{
        thread_pool_job *head;//队列头指针
        thread_pool_job *tail;//队列尾指针
        int num;//任务数目
        sem_t *quene_sem;//信号量
}thread_pool_job_queue;

//线程池(存放消费者进程)
typedef struct thread_pool_s{
        pthread_t *threads;//线程
        int threads_num;//线程数目
        thread_pool_job_queue *job_queue;//指向工作队列的指针
}thread_pool;

//typedef struct thread_data_s{
//      pthread_mutex_t *mutex_t;//互斥量
//      thread_pool *tp_p;//指向线程池的指针
//}thread_data;

//初始化线程池
thread_pool* tp_init(int thread_num);

//初始化工作队列
int tp_job_quene_init(thread_pool *tp);

//向工作队列中添加一个元素
void tp_job_quene_add(thread_pool *tp,thread_pool_job *new_job);

//向线程池中添加一个工作项
int tp_add_work(thread_pool *tp,void *(*func_p)(void *),void *arg);

//取得工作队列的最后个节点
thread_pool_job* tp_get_lastjob(thread_pool *tp);

//删除工作队列的最后个节点
int tp_delete__lastjob(thread_pool *tp);

//销毁线程池
void tp_destroy(thread_pool *tp);

//消费者线程函数
void* tp_thread_func(thread_pool *tp);

//生产者线程执行函数
void* thread_func_producer(thread_pool *tp);

#endif
