#ifndef HEAP
#define HEAP
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <time.h>
using namespace std;

#define BUFFER_SIZE 64
class heap_timer;

struct client_data
{
	sockaddr_in address;
	int sockfd;
	char buf[BUFFER_SIZE];
	heap_timer* timer;
};

class heap_timer
{
	public:
		time_t expire;
		void (*cb_func)(client_data*);
		client_data* user_data;
		heap_timer(int delay);
};

heap_timer:: heap_timer(int delay)
{
	expire = time(NULL) + delay;
}

class time_heap
{
	private:
		int cur_size;//当前元素个数
		int capacity;
		heap_timer** array;

	public:
		//初始化方式1:
		//先生成空堆然后将timer一个一个添加进来
		//time_heap *my_time_heap = new time(1);
		//my_time_heap->add_timer(timer1);
		//my_time_heap->add_timer(timer2);
		//my_time_heap->add_timer(timer3);
		time_heap(int cap) throw (exception) 
			: cur_size(0), capacity(cap)
		{
			int i;
			array = new heap_timer* [capacity];
			if (!array) {
				throw exception();
			}
			for (i = 0; i < capacity; i++) {
				array[i] = NULL;
			}
		}

		//初始化方式2:
		//一次性把无序的定时器数组传进来建立成有序堆
		//cap:数组总长度 
		//size:实际使用长度
		time_heap(heap_timer**  init_array, int size, int cap) throw (exception)
			:cur_size(size), capacity(cap)
		{
			int i;
			if (capacity < size) {
				throw exception();
			}
			array = new heap_timer* [capacity];
			if (!array) {
				throw exception();
			}
			for (i = 0; i < capacity; i++) {
				array[i] = NULL;
			}
			//size 为元素总数
			if (size!=0) {
				for (i = 0; i < cur_size; i++) {
					array[i] = init_array[i];
				}
				for (i = (size-1)/2; i >= 0 ; i--) {
					percolate_down(i);
				}
			}
		}
		~time_heap();

		int add_timer(heap_timer* timer) throw (exception);

		void del_timer(heap_timer* timer);

		void adjust_timer(heap_timer* timer, int delay);

		int find_timer(heap_timer* timer);

		void print_timer(heap_timer * timer);

		heap_timer* top() const;

		void pop_timer();

		void tick();

		bool empty() const;
		
		void debug();

	private:
		void percolate_down(int hole);
		void resize() throw (exception);


};

time_heap::~time_heap()
{
	int i;
	for (i = 0; i < cur_size; i++) {
		delete array[i];
	}
	delete [] array;
}

int time_heap::add_timer(heap_timer* timer) throw (exception)
{
	if (!timer) {
		throw exception();
	}
	if (cur_size>=capacity) {
		resize();
	}
	int hole = cur_size++;
	int parent;

	//hole不是唯一节点
	for(; hole>0; hole=parent) {
		parent = (hole-1)/2;
		if (timer->expire < array[parent]->expire) {
			array[hole] = array[parent];
		}else{
			break;
		}
	}
	array[hole] = timer;
	//cout<<"add_timer hole is :"<<hole<<endl;
	//print_timer(timer);
	//debug();
	return hole;
}

int time_heap::find_timer(heap_timer* timer)
{
	if (empty()) {
		return -1;
	}
	int i;
	for (i = 0; i < cur_size; i++) {
		if (timer == array[i]) {
			return i;
		}
	}
	return -1;
}

//用于处理客户端主动发送EOF,服务器需要del_timer
void time_heap::del_timer(heap_timer* timer)
{
	if (!timer) {
		return;
	}
	//find the num of the timer
	int num;
	if (!empty()) {
		num = find_timer(timer);
	}

	//printf("num:%d\n", num);
	if (num>=0) {
		delete array[num];
		array[num] = array[--cur_size];
		percolate_down(num);
	}
}

void time_heap::adjust_timer(heap_timer* timer, int delay){
	//先找到要调整时间的timer在堆中
	//从堆中移除
	//加时间
	//放回堆
	int num, new_num;
	heap_timer* tmp_timer;
	if (!empty()) {
		num = find_timer(timer);
	}

	//cout <<"++adjust_timer start, timer adderss:"<<timer<<endl;
	//cout <<"find num:"<<num <<endl;
	//debug();

	if (num<0) {
		printf("can not find timer\n");
		exit(1);
	}else{
		tmp_timer = array[num];
		tmp_timer->expire+=delay;
		array[num] = array[--cur_size];
		printf("on adjust\n");
		//debug();
		percolate_down(num);
		new_num = add_timer(tmp_timer);
		//debug();
		//cout <<"new_num:"<<new_num<<" ";
		print_timer(tmp_timer);
	}
	//cout << "++adjust done" <<endl;
}

heap_timer* time_heap:: top() const
{
	if (empty()) {
		return NULL;
	}
	return array[0];
}

void time_heap:: pop_timer()
{
	if (empty()) {
		return;
	}
	if (array[0]) {
		delete array[0];
		array[0] = array[--cur_size];
		percolate_down(0);
	}
}

void time_heap:: tick()
{
	printf("tick worker\n");
	heap_timer *timer = array[0];
	time_t cur = time(NULL);
	while (!empty()) {
		if (!timer) {
			break;
		}
		if (timer->expire > cur) {
			break;
		}
		if (array[0]->cb_func) {
			array[0]->cb_func(array[0]->user_data);
		}
		pop_timer();
		timer = array[0];
	}
}

bool time_heap::empty() const{
	return cur_size == 0;
}

void time_heap::print_timer(heap_timer * timer)
{
	cout <<"address:"<<timer;
	cout <<" new_expire:"<<timer->expire<<endl;
}

void time_heap::debug()
{
	int i;
	cout <<">>debug info:"<<endl;
	cout <<"cur_size:"<<cur_size<<endl;
	if (!empty()) {
		for (i = 0; i < cur_size; i++) {
			cout <<"i:"<<i<<" address:"<<array[i]
				<<" expire:"<<array[i]->expire<<endl;
		}
	}else{
		printf("heap is empty\n");
	}
	cout<<">>debug done"<<endl;
}

void time_heap :: percolate_down(int hole)
{
	heap_timer* timer= array[hole];
	int child;
	//是否有子节点
	while (2*hole+1 <= cur_size-1) {
		child = 2*hole+1;
		//是否有右孩子
		if (child < cur_size-1 && array[child]->expire > array[child+1]->expire) {
			child++;
		}
		if (array[hole] > array[child]) {
			array[hole] = array[child];
		}else{
			break;
		}
		hole = child;
	}
	array[hole] = timer;
}

void  time_heap::resize() throw (exception)
{
	heap_timer** temp = new heap_timer* [2*capacity];
	if (!temp) {
		throw exception();
	}
	int i;
	capacity *=2;
	for (i = 0; i < cur_size; i++) {
		temp[i] = array[i];
	}
	delete [] array;
	array = temp;
}

#endif
