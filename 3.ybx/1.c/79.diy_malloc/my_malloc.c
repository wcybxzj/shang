#include "my_malloc.h"

#define TRUE 1
#define FALSE 0
#define BLOCK_SZIE 40

typedef struct block *block_t;
struct block {
	size_t size;  /* 数据区大小 */
	block_t prev; /* 指向上个块的指针 */
	block_t next; /* 指向下个块的指针 */
	int free;     /* 是否是空闲块 1空闲 0非空闲*/
	int padding;  /* 填充4字节，保证meta块长度为8的倍数 */
	void *magic_ptr;    /* Magic pointer，指向data */
	char  data[1]; /* 这是一个虚拟字段，表示数据块的第一个字节，长度不应计入meta */
};

static block_t first_block=NULL;

static size_t align8(size_t size);
static block_t get_block(void *data_ptr);
static int copy_block_data(block_t src_ptr, block_t dst_ptr);
static int valid_addr(void *data_ptr);

static block_t extend_heap(block_t last, size_t align_size);
static block_t find_block(block_t *last_ptr, size_t size);
static block_t block_split(block_t old_ptr, size_t old_align_size);
static block_t block_merge(block_t block_ptr);

static size_t align8(size_t size){
	if ( (size&0x7) == 0) {
		return size;
	}
	return ((size>>3)+1)<<3;
}

//通过数据指针获取块指针
static block_t get_block(void *data_ptr)
{
	return (block_t) (((char*)data_ptr)-BLOCK_SZIE);
}

//以8个自己为单位进行赋值
static int copy_block_data(block_t src_ptr, block_t dst_ptr)
{
	int i=0, s8=0;
	size_t *src_data=NULL, *dst_data=NULL;
	if (src_ptr->size<=0 || dst_ptr->size<=0) {
		return FALSE;
	}

	if (dst_ptr->size <= src_ptr->size) {
		s8 = align8(dst_ptr->size)>>3;
	}else{
		s8 = align8(src_ptr->size)>>3;
	}

	src_data = (size_t*)src_ptr->data;
	dst_data = (size_t*)dst_ptr->data;
	for (i = 0; i < s8; i++) {
		dst_data[i]=src_data[i];
	}
	return TRUE;
}

static int valid_addr(void *data_ptr)
{
	if (!first_block) {
		return FALSE;
	}
	if ((block_t)data_ptr > first_block && data_ptr < sbrk(0)) {
		if ( data_ptr == ((get_block(data_ptr))->magic_ptr) ) {
			return TRUE;
		}
	}
	return FALSE;
}

//开辟新的block
static block_t extend_heap(block_t last, size_t align_size){
	block_t ptr;
	ptr= sbrk(0);
	if (sbrk(BLOCK_SZIE+align_size) == (void*)-1) {
		return NULL;
	}
	ptr->size=align_size;
	ptr->prev=NULL;
	ptr->next=NULL;
	if (last) {
		last->next = ptr;
		ptr->prev  = last;
	}
	ptr->free=0;
	ptr->magic_ptr = (char*)ptr+BLOCK_SZIE;
	return ptr;
}

//find_block从frist_block开始，查找第一个符合要求的block并返回block起始地址，如果找不到这返回NULL。
//last指针始终指向当前遍历的block,为了如果找不到合适的block而开辟新block使用的
static block_t find_block(block_t *last_ptr, size_t size){
	block_t ptr = (block_t)first_block;
	while (ptr &&!(ptr->free &&ptr->size>=size)) {
		*last_ptr = ptr;
		ptr = ptr->next;
	}
	return ptr;
}

//块初始化
void block_zero(block_t ptr, size_t size)
{
	size_t i = 0;
	char *tmp = NULL;
	tmp = (char *)ptr;
	for (i = 0; i < size; i++) {
		tmp[i]='\0';
	}
}

//用处:找到空闲块后如果使用量太小将大块进行分块
//参数
//old_ptr:老块的头
//old_align_size:老块的实际需要使用的长度
static block_t block_split(block_t old_ptr, size_t old_align_size)
{
	block_t newptr=NULL;
	size_t new_size=0;

	if ((old_ptr->size - old_align_size) < (BLOCK_SZIE+8)) {
		return NULL;
	}

	newptr = (block_t)(old_ptr->data+old_align_size);
	new_size = (old_ptr->size)-(BLOCK_SZIE+old_align_size);
	block_zero(newptr, new_size);

	newptr->free=1;
	newptr->size = new_size;
	newptr->prev = old_ptr;
	newptr->next = old_ptr->next;
	newptr->magic_ptr = (char *)newptr+BLOCK_SZIE;

	old_ptr->size = old_align_size;
	old_ptr->next = newptr;
	return old_ptr;
}

static block_t block_merge(block_t block_ptr)
{
	block_t next_tmp = block_ptr->next;
	if (block_ptr->next && block_ptr->next->free) {
		block_ptr->size += BLOCK_SZIE+block_ptr->next->size;
		block_ptr->next = block_ptr->next->next;
		if (block_ptr->next) {
			block_ptr->next->prev = block_ptr;
		}
		block_zero(next_tmp,BLOCK_SZIE+next_tmp->size);
	}
	return block_ptr;
}

/*
first_block，初始化为NULL
另外，我们需要剩余空间至少有BLOCK_SIZE + 8才执行分裂操作。
我们希望malloc分配的数据区是按8字节对齐，在size不为8的倍数时，要将size8的倍数
*/
void *my_malloc(size_t size)
{
	block_t ptr=NULL;
	block_t last_ptr=NULL;
	size_t align_size;
	align_size = align8(size);

	if (size<=0) {
		return NULL;
	}

	//第一次运行malloc
	if (first_block==NULL) {
		ptr = extend_heap(NULL,align_size);
		if (!ptr) {
			return NULL;
		}
		first_block = ptr;
	}else{
		last_ptr = first_block;
		ptr = find_block(&last_ptr, align_size);
		//直接插到可用的块用
		if (ptr) {
			//可用块的空间大于需要尺寸,进行分裂
			if ( (ptr->size - align_size) >= (BLOCK_SZIE+8)) {
				ptr = block_split(ptr,align_size);
				if (!ptr) {
					NULL;
				}
			}
			ptr->free=0;
		}else{
			ptr = extend_heap(last_ptr, align_size);
			if (!ptr) {
				return NULL;
			}
		}
	}
	return ptr->data;
}

//实现calloc只要两步：
//1.malloc一段内存
//2.将数据区内容置为0
//由于我们的数据区是按8字节对齐的，所以为了提高效率，我们可以每8字节一组置0，不用一个一个字节设置
void* my_calloc(size_t num, size_t size)
{
	size_t *new=NULL;
	size_t s8=0, i=0;
	new = my_malloc(num*size);
	if (!new) {
		return NULL;
	}
	s8 = align8(num*size)>>3;
	for (i = 0; i < s8; i++) {
		new[i]=0;
	}
	return new;
}

//首先检查参数地址的合法性，如果不合法则不做任何事；
//否则，将此block的free标为1
//前边的块能否合并
//后面的block能否合并
//如果当前是最后一个block，则回退break指针释放进程内存，
//如果当前block是最后一个block，则回退break指针并设置first_block为NULL
void my_free(void *ptr)
{
	block_t block_ptr;
	if (!valid_addr(ptr)) {
		return;
	}
	block_ptr = get_block(ptr);
	block_ptr->free = 1;

	if (block_ptr->prev && block_ptr->prev->free) {
		block_ptr = block_merge(block_ptr->prev);
	}

	if (block_ptr->next && block_ptr->next->free) {
		block_ptr = block_merge(block_ptr);
	}

	if (block_ptr->prev==NULL && block_ptr->next==NULL) {
		if (block_ptr==first_block) {
			first_block=NULL;
		}
		brk(block_ptr);
	}else if(block_ptr->next==NULL){
		brk(block_ptr);
	}
}

/*
思路:
if (新快大小和老块一样大) {
	return 老块;
}else if (新快小于老块) {
	进行分裂
	return 分裂后的新块;
}

if (老块的后边有空块 && (老块大小+后边的空块)>=新块大小) {
	进行合并
}else{
	my_malloc申请一个足够大的新块
	copy老块data到新块data
	老块释放
}

*/
void *my_realloc(void *src_data_ptr, size_t size)
{
	block_t src_ptr = NULL;
	block_t tmp_ptr = NULL;
	size_t align_size = 0;

	int ret=0;
	void *dst_data_ptr = NULL;
	block_t dst_ptr = NULL;

	align_size = align8(size);
	if (src_data_ptr==NULL) {
		return my_malloc(align_size);
	}

	src_ptr = get_block(src_data_ptr);
	if (src_ptr->size == align_size) {
		return src_data_ptr;
	}else if ((src_ptr->size - align_size) >=BLOCK_SZIE+8) {
		tmp_ptr = block_split(src_ptr, align_size);
		if (tmp_ptr) {
			return tmp_ptr->data;
		}
	}else if (src_ptr->next && src_ptr->next->free) {
		if ( (src_ptr->size + src_ptr->size + BLOCK_SZIE) >= align_size) {
			return block_merge(src_ptr);
		}
	}

	//新块大于老块,使用my_malloc申请一个足够大的新块
	dst_data_ptr = my_malloc(align_size);
	if (dst_data_ptr==NULL) {
		return NULL;
	}
	dst_ptr = get_block(dst_data_ptr);
	if (dst_ptr==NULL) {
		return NULL;
	}

	//copy老块data到新块data
	ret = copy_block_data(src_ptr, dst_ptr);
	if (!ret) {
		return NULL;
	}

	//老块释放
	my_free(src_data_ptr);

	return dst_data_ptr;
}

size_t get_malloc_size(void *data_ptr){
	block_t ptr= get_block(data_ptr);
	if (!ptr || ptr->free) {
		return -1;
	}
	return ptr->size;
}
