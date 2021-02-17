#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "32.threadpool.h"

//https://blog.csdn.net/xiajun07061225/article/details/11889423
//互斥量,用于对工作队列的访问
static pthread_mutex_t mutex =   PTHREAD_MUTEX_INITIALIZER;

//标记线程池是否处于可用状态
static int tp_alive = 1;

//初始化线程池
thread_pool* tp_init(int thread_num){
	thread_pool *tp;
	int i;

	if(thread_num < 1)
		thread_num = 1;

	tp = (thread_pool *)malloc(sizeof(thread_pool));
	//判断内存分配是否成功
	if(NULL == tp){
		printf("ERROR:allocate memory for thread_pool failed\n");
		return NULL;
	}
	tp->threads_num = thread_num;
	//分配线程所占内存空间
	tp->threads = (pthread_t*)malloc(thread_num * sizeof(pthread_t));
	//判断内存分配是否成功
	if(NULL == tp->threads){
		printf("ERROR:allocate memory for threads in thread pool failed\n");
		return NULL;
	}

	if(tp_job_quene_init(tp))
		return NULL;

	tp->job_queue->quene_sem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(tp->job_queue->quene_sem,0,0);//信号量初始化

	//初始化线程
	for(i = 0;i < thread_num;++i){
		pthread_create(&(tp->threads[i]),NULL,(void *)tp_thread_func,(void *)tp);
	}

	return tp;
}

//初始化工作队列
int tp_job_quene_init(thread_pool *tp){
	tp->job_queue = (thread_pool_job_queue *)malloc(sizeof(thread_pool_job_queue));

	if(NULL == tp->job_queue){
		return -1;
	}

	tp->job_queue->head = NULL;
	tp->job_queue->tail = NULL;
	tp->job_queue->num = 0;

	return 0;
}

//线程函数
void* tp_thread_func(thread_pool *tp){
	FUNC function;
	void *arg_buf;
	thread_pool_job *job_p;

	while(tp_alive){
		//线程阻塞,等待信号量
		if(sem_wait(tp->job_queue->quene_sem)){
			printf("thread waiting for semaphore....\n");
			exit(1);
		}
		if(tp_alive){
			pthread_mutex_lock(&mutex);
			job_p = tp_get_lastjob(tp);
			if(NULL == job_p){
				pthread_mutex_unlock(&mutex);
				continue;
			}
			function = job_p->function;
			arg_buf = job_p->arg;
			if(tp_delete__lastjob(tp))
				return NULL;
			pthread_mutex_unlock(&mutex);
			//运行指定的线程函数
			printf("consumer...get a job from job quene and run it!\n");
			function(arg_buf);
			free(job_p);
		}
		else
			return NULL;
	}

	return;
}

//向工作队列中添加一个元素
void tp_job_quene_add(thread_pool *tp,thread_pool_job *new_job){
	new_job->pre = NULL;
	new_job->next = NULL;
	thread_pool_job *old_head_job = tp->job_queue->head;

	if(NULL == old_head_job){
		tp->job_queue->head = new_job;
		tp->job_queue->tail = new_job;
	}
	else{
		old_head_job->pre = new_job;
		new_job->next = old_head_job;
		tp->job_queue->head = new_job;
	}

	++(tp->job_queue->num);

	sem_post(tp->job_queue->quene_sem);
}

//取得工作队列的最后一个节点
thread_pool_job* tp_get_lastjob(thread_pool *tp){
	return tp->job_queue->tail;
}

//删除工作队列的最后个节点
int tp_delete__lastjob(thread_pool *tp){
	if(NULL == tp)
		return -1;

	thread_pool_job *last_job = tp->job_queue->tail;
	if(0 == tp->job_queue->num){
		return -1;
	}
	else if(1 == tp->job_queue->num){
		tp->job_queue->head = NULL;
		tp->job_queue->tail = NULL;
	}
	else{
		last_job->pre->next = NULL;
		tp->job_queue->tail = last_job->pre;
	}

	//修改相关变量
	--(tp->job_queue->num);

	return 0;
}

//向线程池中添加一个工作项
int tp_add_work(thread_pool *tp,void *(*func_p)(void *),void *arg){
	thread_pool_job *new_job = (thread_pool_job *)malloc(sizeof(thread_pool_job));
	if(NULL == new_job){
		printf("ERROR:allocate memory for new job failed!\n");
		exit(1);
	}
	new_job->function = func_p;
	new_job->arg = arg;
	pthread_mutex_lock(&mutex);
	tp_job_quene_add(tp,new_job);
	pthread_mutex_unlock(&mutex);
}


//销毁线程池
void tp_destroy(thread_pool *tp){
	int i;
	tp_alive = 0;

	//等待线程运行结束
	//sleep(10);
	for(i = 0;i < tp->threads_num;++i){
		pthread_join(tp->threads[i],NULL);
	}
	free(tp->threads);

	if(sem_destroy(tp->job_queue->quene_sem)){
		printf("ERROR:destroy semaphore failed!\n");
	}
	free(tp->job_queue->quene_sem);

	//删除ｊｏｂ队列
	thread_pool_job *current_job = tp->job_queue->tail;
	while(tp->job_queue->num){
		tp->job_queue->tail = current_job->pre;
		free(current_job);
		current_job = tp->job_queue->tail;
		--(tp->job_queue->num);
	}
	tp->job_queue->head = NULL;
	tp->job_queue->tail = NULL;
}

//自定义线程执行函数
void* thread_func1(){
	printf("Task1 running...by Thread  :%u\n",(unsigned int)pthread_self());
}

//自定义线程执行函数
void* thread_func2(){
	printf("Task2 running...by Thread  :%u\n",(unsigned int)pthread_self());
}

//生产者线程执行函数
void* thread_func_producer(thread_pool *tp){
	while(1){
		printf("producer...add a job(job1) to job quene!\n");
		tp_add_work(tp,(void*)thread_func1,NULL);
		sleep(1);
		printf("producer...add a job(job2) to job quene!\n");
		tp_add_work(tp,(void*)thread_func2,NULL);
	}
}

int main(){
	thread_pool *tp = tp_init(5);
	int i;
	int arg = 7;
	pthread_t producer_thread_id;//生产者线程ID
	pthread_create(&producer_thread_id,NULL,(void *)thread_func_producer,(void *)tp);

	pthread_join(producer_thread_id,NULL);
	tp_destroy(tp);

	return 0;
}
