#include "server.h"

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>
#include "intset.h"

//模拟 sunionDiffGenericCommand()方法
void ybx_sunionDiffGenericCommand(robj **sets,int setnum, int op){
	long long target;
	setTypeIterator *si;
	robj *ele, *dstset = NULL;
	int j, cardinality = 0;
	int diff_algo = 1;
	if (op == SET_OP_DIFF && sets[0]) {
		long long algo_one_work = 0, algo_two_work = 0;
		for (j = 0; j < setnum; j++) {
			if (sets[j] == NULL) continue;
			algo_one_work += setTypeSize(sets[0]);
			algo_two_work += setTypeSize(sets[j]);
		}
		//printf("one:%lld, two:%lld\n",algo_one_work, algo_two_work);
		algo_one_work /= 2;
		int diff_algo = (algo_one_work <= algo_two_work) ? 1 : 2;
		if (diff_algo == 1 && setnum > 1) {
			qsort(sets+1,setnum-1,sizeof(robj*),
					qsortCompareSetsByRevCardinality);
		}
	}

    // 创建一个临时集合对象作为结果集
    dstset = createIntsetObject();

	if (op == SET_OP_UNION) {
		//TODO
	} else if (op == SET_OP_DIFF && sets[0] && diff_algo == 1) {
		si = setTypeInitIterator(sets[0]);
		while((ele = setTypeNextObject(si)) != NULL) {
			for (j = 1; j < setnum; j++) {
				if (!sets[j]) continue; /* no key is an empty set. */   //集合键不存在跳过本次循环
				if (sets[j] == sets[0]) break; /* same set! */          //相同的集合没必要比较
				if (setTypeIsMember(sets[j],ele)) break;                //如果元素存在后面的集合中，遍历下一个元素
			}
			if (j == setnum) {
				setTypeAdd(dstset,ele);
				cardinality++;
			}
			//print_obj_encoding(ele);
			decrRefCount(ele);  //释放元素对象空间
		}
		setTypeReleaseIterator(si); //释放迭代器空间
	} else if (op == SET_OP_DIFF && sets[0] && diff_algo == 2) {
		// 执行差集操作并且使用算法2
		// 将第一个集合的所有元素加入到结果集中，然后遍历其后的所有集合，将有交集的元素从结果集中删除
		// 时间复杂度O(N)，N是所有集合中元素的总个数
		// 遍历所有的集合
		for (j = 0; j < setnum; j++) {
			if (!sets[j]) continue; /* non existing keys are like empty sets */
			si = setTypeInitIterator(sets[j]);
			// 创建集合类型迭代器遍历每一个集合中的所有元素
			while((ele = setTypeNextObject(si)) != NULL) {
				// 如果是第一个集合，将每一个元素加入到结果集中
				if (j == 0) {
					if (setTypeAdd(dstset,ele)) cardinality++;
					// 如果是其后的集合，将当前元素从结果集中删除，如结果集中有的话
				} else {
					if (setTypeRemove(dstset,ele)) cardinality--;
				}
				decrRefCount(ele);
			}
			setTypeReleaseIterator(si);//释放迭代器空间

			/* Exit if result set is empty as any additional removal
			 * of elements will have no effect. */
			// 只要结果集为空，那么差集结果就为空，不用比较后续的集合
			if (cardinality == 0) break;
		}
	}

	// 遍历结果集中的每一个元素，并发送给client
	si = setTypeInitIterator(dstset);
	
	while((ele = setTypeNextObject(si)) != NULL) {
		//print_obj_encoding(ele);
		if (ele->encoding ==OBJ_ENCODING_INT) {
			if (getLongLongFromObject(ele, &target) == C_OK) {
				printf("%lld\n",target);
			}else{
				printf("不能识别此元素\n");
			}
		}else if(ele->encoding ==OBJ_ENCODING_RAW || ele->encoding== OBJ_ENCODING_EMBSTR){
			printf("%s\n", ele->ptr);
		}
		decrRefCount(ele);  //发送完要释放空间
	}
	setTypeReleaseIterator(si); //释放迭代器

	decrRefCount(dstset);       //发送集合后要释放结果集的空间
}

// 设定集合key1的元素
// 127.0.0.1:6379> SADD key1 1 2 3 4 5
// (integer) 5
// // 设定集合key2的元素
// 127.0.0.1:6379> SADD key2 3 4 5 6 7
// (integer) 5
// // 求集合key1和key2的差集
// 127.0.0.1:6379> SDIFF key1 key2
// 1) "1"
// 2) "2"
// // 求集合key1和key2的并集
// 127.0.0.1:6379> SUNION key1 key2
// 1) "1"
// 2) "2"
// 3) "3"
// 4) "4"
// 5) "5"
// 6) "6"
// 7) "7"
void sdiff()
{
	int i;
	robj *int_obj;
	robj *set_objs[3];

	int_obj = createStringObjectFromLongLong(1);
	set_objs[0] = setTypeCreate(int_obj);
	set_objs[1] = setTypeCreate(int_obj);
	set_objs[2] = setTypeCreate(int_obj);

	int arr0[]={1,2,3,4};//4
	int arr1[]={3,5,6,7,8,9,10};//7
	int arr2[]={1,5,6,7,11,12,13,14};//8
	int n0 =sizeof(arr0)/sizeof(*arr0);
	int n1 =sizeof(arr1)/sizeof(*arr1);
	int n2 =sizeof(arr2)/sizeof(*arr2);

	for (i = 0; i < n0; i++) {
		int_obj = createStringObjectFromLongLong(arr0[i]);
		setTypeAdd(set_objs[0], int_obj);
	}

	for (i = 0; i < n1; i++) {
		int_obj = createStringObjectFromLongLong(arr1[i]);
		setTypeAdd(set_objs[1], int_obj);
	}

	for (i = 0; i < n2; i++) {
		int_obj = createStringObjectFromLongLong(arr2[i]);
		setTypeAdd(set_objs[2], int_obj);
	}

	ybx_sunionDiffGenericCommand(set_objs, 3, SET_OP_DIFF);
}


void sunion()
{
	/* code */
}

int main(int argc, const char *argv[])
{
	init();


	sdiff();

	return 0;
}

