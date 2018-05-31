#define MAXSIZE 20
typedef int KeyType;

typedef struct _RedType {
	KeyType key;
	InfoType otherinfo;
} RedType;

typedef struct _SqList {
	RedType r[MAXSIZE+1];//0号元素闲置
	int length;
} SqList;
