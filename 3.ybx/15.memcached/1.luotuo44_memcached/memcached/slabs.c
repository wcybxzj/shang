#include "memcached.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

static pthread_mutex_t slabs_lock = PTHREAD_MUTEX_INITIALIZER;

//slabclass 划分数据空间 
//1.Memcached在启动的时候，会初始化一个slabclass数组，该数组用于存储最大200个slabclass_t的数据结构体。
//2.Memcached并不会将所有大小的数据都会放置在一起，而是预先将数据空间划分为一系列的slabclass_t。
//3.每个slabclass_t，都只存储一定大小范围的数据。
//slabclass数组中，前一个slabclass_t可以存储的数据大小要小于下一个slabclass_t结构可以存储的数据大小。
//4.例如：slabclass[3]只存储大小介于120 （slabclass[2]的最大值）到 150 bytes的数据。
//如果一个数据大小为134byte将被分配到slabclass[3]中。
//5.memcached默认情况下下一个slabclass_t存储数据的最大值为前一个的1.25倍（settings.factor），这个可以通过修 改-f参数来修改增长比例

//确定slab分配器的分配规格：
//现在来看一下memcached是怎么确定slab分配器的分配规格的。因为memcached使用了全局变量，先来看一下全局变量
typedef struct {  
    //当前的slabclass存储最大多大的item  
    unsigned int size;//slab分配器分配的item的大小     
    //每一个slab上可以存储多少个item.每个slab大小为1M，可以存储的item个数根据(1024*1024)/size决定。  
    unsigned int perslab; //每一个slab分配器能分配多少个item

    //当前slabclass的（空闲item列表）freelist 的链表头部地址  
    //freelist的链表是通过item结构中的item->next和item->prev连建立链表结构关系  
    void *slots;  //指向空闲item链表  

    //当前总共剩余多少个空闲的item  
    //当sl_curr=0的时候，说明已经没有空闲的item，需要分配一个新的slab（每个1M，可以切割成N多个Item结构）  
    unsigned int sl_curr;   //空闲item的个数  
  
    //这个是已经分配了内存的slabs个数。list_size是这个slabs数组(slab_list)的大小    
    unsigned int slabs; //本slabclass_t可用的slab分配器个数     
    //slab数组，数组的每一个元素就是一个slab分配器，这些分配器都分配相同尺寸的内存  
    void **slab_list;     
    unsigned int list_size; //slab数组的大小, list_size >= slabs  
  
    //用于reassign，指明slabclass_t中的哪个块内存要被其他slabclass_t使用  
    unsigned int killing;   
  
    size_t requested; //本slabclass_t分配出去的字节数  
} slabclass_t;

//数组元素虽然有MAX_NUMBER_OF_SLAB_CLASSES个，但实际上并不是全部都使用的。  
//实际使用的元素个数由power_largest指明  

//定义了一个全局slabclass数组。
//这个数组就是前面那些图的slabclass_t数组。
//虽然slabclass数组有201个元素,第1个元素不使用所以最多可以使用的元素个数200个,但可能并不会所有元素都使用的。
//由全局变量power_largest指明使用了多少个元素.
static slabclass_t slabclass[MAX_NUMBER_OF_SLAB_CLASSES];//201  

static size_t mem_limit = 0;//用户设置的内存最大限制  
static size_t mem_malloced = 0;  
static int power_largest;//slabclass数组中,已经使用了的元素个数.  
  
//如果程序要求预先分配内存，而不是到了需要的时候才分配内存，那么  
//mem_base就指向那块预先分配的内存.  
//mem_current指向还可以使用的内存的开始位置  
//mem_avail指明还有多少内存是可以使用的  
static void *mem_base = NULL;  
static void *mem_current = NULL;  
static size_t mem_avail = 0;  


//向内存池申请内存：
//与do_slabs_free函数对应的是do_slabs_alloc函数。
//当worker线程向内存池申请内存时就会调用该函数。
//在调用之前就要根据所申请的内存大小，确定好要向slabclass数组的哪个元素申请内存了。
//函数slabs_clsid就是完成这个任务。

//slabs_clsid - 查询slabclass的ID
//slabs_clsid方法，主要通过item的长度来查询应该适合存放到哪个slabsclass_t上面。
unsigned int slabs_clsid(const size_t size) {//返回slabclass索引下标值  
    int res = POWER_SMALLEST;//res的初始值为1  

    //slabclass这个结构上的size会存储该class适合多大的item存储  
    //例如  
    //slabclass[0] 存储96byte  
    //slabclass[1] 存储120byte  
    //slabclass[2] 存储150byte  
    //则，如果存储的item等于109byte，则存储在slabclass[1]上 

    //返回0表示查找失败，因为slabclass数组中，第一个元素是没有使用的  
    if (size == 0)  
        return 0;  
      
    //因为slabclass数组中各个元素能分配的item大小是升序的  
    //所以从小到大直接判断即可在数组找到最小但又能满足的元素  
    while (size > slabclass[res].size)  
        if (res++ == power_largest)     /* won't fit in the biggest slab */  
            return 0;  
    return res;  
}//end nlabs_clsid() 

//代码中出现的item是用来存储我们放在memcached的数据。
//代码中的循环决定了slabclass数组中的每一个slabclass_t能分配的item大小，
//也就是slab分配器能分配的item大小，同时也确定了slab分配器能分配的item个数。

//可以通过增大settings.item_size_max而使得memcached可以存储更大的一条数据信息。
//当然是有限制的，最大也只能为128MB。巧的是，slab分配器能分配的最大内存也是受这个settings.
//item_size_max所限制。
//因为每一个slab分配器能分配的最大内存有上限，所以slabclass数组中的每一个slabclass_t都有多个slab分配器，其用一个数组管理这些slab分配器。
//而这个数组大小是不受限制的，所以对于某个特定的尺寸的item是可以有很多很多的。
//当然整个memcached能分配的总内存大小也是有限制的，可以在启动memcached的时候通过-m选项设置，默认值为64MB。
//slabs_init函数中的limit参数就是memcached能分配的总内存。

//参数factor是扩容因子，默认值是1.25  

//slabs_init - slabclass的初始化
//slabs_init方法主要用于初始化slabclass数组结构。

/**
 * Determines the chunk sizes and initializes the slab class descriptors
 * accordingly.
 */
void slabs_init(const size_t limit, const double factor, const bool prealloc) {
    int i = POWER_SMALLEST - 1;
    //settings.chunk_size默认值为48，可以在启动memcached的时候通过-n选项设置  
    //size由两部分组成: item结构体本身 和 这个item对应的数据  
    //这里的数据也就是set、add命令中的那个数据.后面的循环可以看到这个size变量会  
    //根据扩容因子factor慢慢扩大，所以能存储的数据长度也会变大的 
    unsigned int size = sizeof(item) + settings.chunk_size;

    mem_limit = limit;////用户设置或者默认的内存最大限制

    //用户要求预分配一大块的内存，以后需要内存，就向这块内存申请。  
    if (prealloc) {//默认值为false
        /* Allocate everything in a big chunk with malloc */
        mem_base = malloc(mem_limit);
        if (mem_base != NULL) {
            mem_current = mem_base;
            mem_avail = mem_limit;
        } else {
            fprintf(stderr, "Warning: Failed to allocate requested memory in"
                    " one large chunk.\nWill allocate in smaller chunks\n");
        }
    }

    //初始化数组，这个操作很重要，数组中所有元素的成员变量值都为0了  
	//slabclass数组中的第一个元素并不使用  
    memset(slabclass, 0, sizeof(slabclass));

    //factor 默认等于1.25 ，也就是说前一个slabclass允许存储96byte大小的数据，  
    //则下一个slabclass可以存储120byte  
    while (++i < POWER_LARGEST && size <= settings.item_size_max / factor) {
        /* Make sure items are always n-byte aligned */
        if (size % CHUNK_ALIGN_BYTES) //8字节对齐
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);

        //这个slabclass的slab分配器能分配的item大小  
        slabclass[i].size = size;
        //这个slabclass的slab分配器最多能分配多少个item(也决定了最多分配多少内存)  
        slabclass[i].perslab = settings.item_size_max / slabclass[i].size;
        size *= factor;
        if (settings.verbose > 1) {
            fprintf(stderr, "slab class %3d: chunk size %9u perslab %7u\n",
                    i, slabclass[i].size, slabclass[i].perslab);
        }
    }

	//最大的item 
    power_largest = i;
    slabclass[power_largest].size = settings.item_size_max;
    slabclass[power_largest].perslab = 1;
    if (settings.verbose > 1) {
        fprintf(stderr, "slab class %3d: chunk size %9u perslab %7u\n",
                i, slabclass[i].size, slabclass[i].perslab);
    }

    /* for the test suite:  faking of how much we've already malloc'd */
    {
        char *t_initial_malloc = getenv("T_MEMD_INITIAL_MALLOC");
        if (t_initial_malloc) {
            mem_malloced = (size_t)atol(t_initial_malloc);
        }

    }

    if (prealloc) {//预分配内存
        slabs_preallocate(power_largest);
    }
}//end slab_init()

//预分配内存：
//现在就假设用户需要预先分配一些内存，而不是等到客户端发送存储数据命令的时候才分配内存。
//slabs_preallocate函数是为slabclass数组中每一个slabclass_t元素预先分配一些空闲的item。
//由于item可能比较小(上面的代码也可以看到这一点)，所以不能以item为单位申请内存（这样很容易造成内存碎片）。
//于是在申请的使用就申请一个比较大的一块内存，然后把这块内存划分成一个个的item，这样就等于申请了多个item。
//本文将申请得到的这块内存称为内存页，也就是申请了一个页。
//如果全局变量settings.slab_reassign为真，那么页的大小为settings.item_size_max，否则等于slabclass_t.size * slabclass_t.perslab。
//settings.slab_reassign主要用于平衡各个slabclass_t的。后文将统一使用内存页、页大小称呼这块分配内存，不区分其大小。

//现在就假设用户需要预先分配内存，看一下slabs_preallocate函数。
//该函数的参数值为使用到的slabclass数组元素个数。slabs_preallocate函数的调用是分配slab内存块和和设置item的。
static void slabs_preallocate (const unsigned int maxslabs) {
    int i;
    unsigned int prealloc = 0;

    /* pre-allocate a 1MB slab in every size class so people don't get
       confused by non-intuitive "SERVER_ERROR out of memory"
       messages.  this is the most common question on the mailing
       list.  if you really don't want this, you can rebuild without
       these three lines.  */

    for (i = POWER_SMALLEST; i <= POWER_LARGEST; i++) {
        if (++prealloc > maxslabs)
            return;
        if (do_slabs_newslab(i) == 0) {
            fprintf(stderr, "Error while preallocating slab memory!\n"
                "If using -L or other prealloc options, max memory must be "
                "at least %d megabytes.\n", power_largest);
            exit(1);
        }
    }
}//end slabs_preallocate()





//增加slab_list成员指向的内存，也就是增大slab_list数组。使得可以有更多的slab分配器  
//除非内存分配失败，否则都是返回1,无论是否真正增大了  
static int grow_slab_list (const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    if (p->slabs == p->list_size) {//用完了之前申请到的slab_list数组的所有元素
        size_t new_size =  (p->list_size != 0) ? p->list_size * 2 : 16;
        void *new_list = realloc(p->slab_list, new_size * sizeof(void *));
        if (new_list == 0) return 0;
        p->list_size = new_size;
        p->slab_list = new_list;
    }
    return 1;
}

//将ptr指向的内存页划分成一个个的item  
static void split_slab_page_into_freelist(char *ptr, const unsigned int id) {  
    slabclass_t *p = &slabclass[id];  
    int x;  
    for (x = 0; x < p->perslab; x++) {  
        //将ptr指向的内存划分成一个个的item.一共划成perslab个  
        //并将这些item前后连起来。  
        //do_slabs_free函数本来是worker线程向内存池归还内存时调用的。但在这里  
        //新申请的内存也可以当作是向内存池归还内存。把内存注入内存池中  
        do_slabs_free(ptr, 0, id);  
        ptr += p->size;//size是item的大小  
    }  
}//end  split_slab_page_into_freelist()  

//slabclass_t中slab的数目是慢慢增多的。该函数的作用就是为slabclass_t申请多一个slab  
//参数id指明是slabclass数组中的那个slabclass_t  

//do_slabs_newslab函数内部调用了三个函数。
//函数grow_slab_list的作用是增大slab数组的大小(如下图所示的slab数组)。
//memory_allocate函数则是负责申请大小为len字节的内存。
//而函数split_slab_page_into_freelist则负责把申请到的内存切分成多个item，并且把这些item用指向连起来，形成双向链表。
static int do_slabs_newslab(const unsigned int id) {  
    slabclass_t *p = &slabclass[id];  
    //settings.slab_reassign的默认值为false，这里就采用false。  
    int len = settings.slab_reassign ? settings.item_size_max  
        : p->size * p->perslab;//其积 <= settings.item_size_max  
    char *ptr;  
  
    //mem_malloced的值通过环境变量设置，默认为0  
    if ((mem_limit && mem_malloced + len > mem_limit && p->slabs > 0) ||  
        (grow_slab_list(id) == 0) ||//增长slab_list(失败返回0)。一般都会成功,除非无法分配内存  
        ((ptr = memory_allocate((size_t)len)) == 0)) {//分配len字节内存(也就是一个页)  
        return 0;  
    }  
  
    memset(ptr, 0, (size_t)len);//清零内存块是必须的  
    //将这块内存切成一个个的item，当然item的大小有id所控制  
    split_slab_page_into_freelist(ptr, id);  
  
    //将分配得到的内存页交由slab_list掌管  
    p->slab_list[p->slabs++] = ptr;  
    mem_malloced += len;  
  
    return 1;  
}//end do_slabs_newslab()

//在do_slabs_alloc函数中如果对应的slabclass_t有空闲的item，那么就直接将之分配出去。
//否则就需要扩充slab得到一些空闲的item然后分配出去。代码如下面所示：

//向slabclass申请一个item。在调用该函数之前，已经调用slabs_clsid函数确定  
//本次申请是向哪个slabclass_t申请item了，参数id就是指明是向哪个slabclass_t  
//申请item。如果该slabclass_t是有空闲item，那么就从空闲的item队列中分配一个  
//如果没有空闲item，那么就申请一个内存页。再从新申请的页中分配一个item  
//返回值为得到的item，如果没有内存了，返回NULL  

//可以看到在do_slabs_alloc函数的内部也是通过调用do_slabs_newslab增加item的。

//do_slabs_alloc - 分配一个item
//1. Memcached分配一个item，会先检查freelist空闲的列表中是否有空闲的item，
//如果有的话就用空闲列表中的item。
//2. 如果空闲列表没有空闲的item可以分配，则Memcached会去申请一个slab（默认大小为1M）的内存块，
//如果申请失败，则返回NULL，表明分配失败。
//3. 如果申请成功，则会去将这个1M大小的内存块，根据slabclass_t可以存储的最大的item的size，
//将slab切割成N个item，然后放进freelist（空闲列表中）
//4. 然后去freelist（空闲列表）中取出一个item来使用。
static void *do_slabs_alloc(const size_t size, unsigned int id) {  
    slabclass_t *p;  
    void *ret = NULL;  
    item *it = NULL;  
  
    if (id < POWER_SMALLEST || id > power_largest) {//下标越界  
        MEMCACHED_SLABS_ALLOCATE_FAILED(size, 0);  
        return NULL;  
    }  
  
    p = &slabclass[id];  
    assert(p->sl_curr == 0 || ((item *)p->slots)->slabs_clsid == 0);  
      
    //如果p->sl_curr等于0，就说明该slabclass_t没有空闲的item了。  
    //此时需要调用do_slabs_newslab申请一个内存页  
    if (! (p->sl_curr != 0 || do_slabs_newslab(id) != 0)) {  
        //当p->sl_curr等于0并且do_slabs_newslab的返回值等于0时，进入这里  
        /* We don't have more memory available */  
        ret = NULL;  
    } else if (p->sl_curr != 0) {  
    //除非do_slabs_newslab调用失败，否则都会来到这里.无论一开始sl_curr是否为0。  
    //p->slots指向第一个空闲的item，此时要把第一个空闲的item分配出去  
      
        /* return off our freelist */  
        it = (item *)p->slots;  
        p->slots = it->next;//slots指向下一个空闲的item  
        if (it->next) it->next->prev = 0;  
        p->sl_curr--;//空闲数目减一  
        ret = (void *)it;  
    }  
  
    if (ret) {  
        p->requested += size;//增加本slabclass分配出去的字节数  
    }  
  
    return ret;  
}// end do_slabs_alloc()

//worker线程向内存池归还内存时，该函数也是会被调用的。
//因为同一slab内存块中的各个item归还时间不同，所以memcached运行一段时间后，
//item链表就会变得很混乱
static void do_slabs_free(void *ptr, const size_t size, unsigned int id) {  
    slabclass_t *p;  
    item *it;  
  
    assert(((item *)ptr)->slabs_clsid == 0);  
    assert(id >= POWER_SMALLEST && id <= power_largest);  
    if (id < POWER_SMALLEST || id > power_largest)  
        return;  
  
    p = &slabclass[id];  
  
    it = (item *)ptr;  
    //为item的it_flags添加ITEM_SLABBED属性，标明这个item是在slab中没有被分配出去  
    it->it_flags |= ITEM_SLABBED;  
  
    //由split_slab_page_into_freelist调用时，下面4行的作用是  
    //让这些item的prev和next相互指向，把这些item连起来.  
    //当本函数是在worker线程向内存池归还内存时调用，那么下面4行的作用是,  
    //使用链表头插法把该item插入到空闲item链表中。  
    it->prev = 0;  
    it->next = p->slots;  
    if (it->next) it->next->prev = it;  
    p->slots = it;//slot变量指向第一个空闲可以使用的item  
  
    p->sl_curr++;//空闲可以使用的item数量  
    p->requested -= size;//减少这个slabclass_t分配出去的字节数  
    return;  
}//end do_slabs_free()

//申请分配内存，如果程序是有预分配内存块的，就向预分配内存块申请内存  
//否则调用malloc分配内存  
static void *memory_allocate(size_t size) {  
    void *ret;  
  
    //如果程序要求预先分配内存，而不是到了需要的时候才分配内存，那么  
    //mem_base就指向那块预先分配的内存.  
    //mem_current指向还可以使用的内存的开始位置  
    //mem_avail指明还有多少内存是可以使用的  
    if (mem_base == NULL) {//不是预分配内存  
        /* We are not using a preallocated large memory chunk */  
        ret = malloc(size);  
    } else {  
        ret = mem_current;  
  
        //在字节对齐中，最后几个用于对齐的字节本身就是没有意义的(没有被使用起来)  
        //所以这里是先计算size是否比可用的内存大，然后才计算对齐  
  
        if (size > mem_avail) {//没有足够的可用内存  
            return NULL;  
        }  
  
        //现在考虑对齐问题，如果对齐后size 比mem_avail大也是无所谓的  
        //因为最后几个用于对齐的字节不会真正使用  
        /* mem_current pointer _must_ be aligned!!! */  
        if (size % CHUNK_ALIGN_BYTES) {//字节对齐.保证size是CHUNK_ALIGN_BYTES (8)的倍数  
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);  
        }  
  
  
        mem_current = ((char*)mem_current) + size;  
        if (size < mem_avail) {  
            mem_avail -= size;  
        } else {//此时，size比mem_avail大也无所谓  
            mem_avail = 0;  
        }  
    }  
  
    return ret;  
}//end memory_allocate()

//作为memcached这个用锁大户，有点不正常。
//其实前面的代码中，有一些是要加锁才能访问的，比如do_slabs_alloc函数。
//之所以上面的代码中没有看到，是因为memcached使用了包裹函数
//(这个概念对应看过《UNIX网络编程》的读者来说很熟悉吧)。
//memcached在包裹函数中加锁后，才访问上面的那些函数的。下面就是两个包裹函数。

//分配一个Item
void *slabs_alloc(size_t size, unsigned int id) {  
    void *ret;  
    pthread_mutex_lock(&slabs_lock);  
    //size：需要分配的item的长度  
    //id：需要分配在哪个slab class上面  
    ret = do_slabs_alloc(size, id);  
    pthread_mutex_unlock(&slabs_lock);  
    return ret;  
}//end slabs_alloc()
  
void slabs_free(void *ptr, size_t size, unsigned int id) {  
    pthread_mutex_lock(&slabs_lock);  
    do_slabs_free(ptr, size, id);  
    pthread_mutex_unlock(&slabs_lock);  
}//end slabs_free()  
