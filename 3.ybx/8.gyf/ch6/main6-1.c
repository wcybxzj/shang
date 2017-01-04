//#define CHAR 1 //char
#define CHAR 0 //int
#include "c1.h"

#if CHAR
	typedef char  TElemType;
	TElemType Nil =' ';
#else
	typedef  int TElemType;
	TElemType Nil = 0;
#endif

#include "c6-1.h"
#include "bo6-1.c"

Status visit(TElemType e)
{
	printf("%d ", e);
	return OK;
}

/*
./main6-1 
create t1
按照层序请输入节点值 int, 0为空， 999为结束 最多输入100个节点
1 2 3 4 5 0 6 7 999
not empty
tree depth:4
have root,root is 1
level:
1 2 3 4 5 6 7 
pre:
1 2 4 7 5 3 6 
in:
7 4 2 5 1 3 6 
post:
7 4 5 2 6 3 1 
输入第几层 第几个节点
2 2
其值为:3
输入想要此值修成什么值
8
pre:
1 2 4 7 5 8 6 
此节点的双亲节点:1
左右孩子分别为:0,6
左右兄弟分别为:2,0
create 右子树为空的树 t2
按照层序请输入节点值 int, 0为空， 999为结束 最多输入100个节点
10 11 0 13 14 0 0 17 999
t2插入t1中,请输入t1的那个双亲节点(要的是值), 插入在左还是右(0左 1右) 
2 1
level:1
order:1, value:1

level:2
order:1, value:2
order:2, value:8

level:3
order:1, value:4
order:2, value:10
order:4, value:6

level:4
order:1, value:7
order:3, value:11
order:4, value:5

level:5
order:5, value:13
orlue:7
order:4, value:5

清除树t1
empty
tree depth:0
not have 
*/
int main(int argc, const char *argv[])
{
	position p;
	int ret, num;
	TElemType node;
	SqBiTree t1, t2;
	int lr;

	printf("create t1\n");
	InitBiTree(t1);
	CreateBiTree(t1);

	ret = BiTreeEmpty(t1);
	if (ret == TRUE) {
		printf("empty\n");
	}else{
		printf("not empty\n");
	}

	num = BiTreeDepth(t1);
	printf("tree depth:%d\n", num);

	ret = Root(t1, &node);
	if (ret == OK) {
		printf("have root,root is %d\n", node);
	}else{
		printf("not have root \n");
	}

	printf("level:\n");
	Level_order(t1, visit);
	printf("pre:\n");
	Pre_order(t1, visit);
	printf("in:\n");
	In_order(t1, visit);
	printf("post:\n");
	Post_order(t1, visit);

	printf("输入第几层 第几个节点\n");
	scanf("%d%d",&p.level, &p.order);
	node = Value(t1, p);
	printf("其值为:%d\n", node);

	printf("输入想要此值修成什么值\n");
	scanf("%d", &node);
	Assign(t1, p, node);

	printf("pre:\n");
	Pre_order(t1, visit);
	
	printf("此节点的双亲节点:%d\n", Parent(t1, node));
	printf("左右孩子分别为:%d,%d\n", LeftChild(t1, node), RightChild(t1, node));
	printf("左右兄弟分别为:%d,%d\n", LeftSibling(t1,node), RightSibling(t1, node));
	
	printf("create 右子树为空的树 t2\n");
	InitBiTree(t2);
	CreateBiTree(t2);

	printf("t2插入t1中,请输入t1的那个双亲节点(要的是值), 插入在左还是右(0左 1右) \n");
	scanf("%d%d", &node, &lr);
	Insert_child(t1, node, lr, t2);
	Print(t1);

	printf("删除t1中的子树 请输入层号 序号 左右\n");
	scanf("%d%d%d", &p.level, &p.order, &lr);
	Delete_child1(t1, p , lr);
	Print(t1);

	printf("清除树t1\n");
	ClearBiTree(t1);

	ret = BiTreeEmpty(t1);
	if (ret == TRUE) {
		printf("empty\n");
	}else{
		printf("not empty\n");
	}

	num = BiTreeDepth(t1);
	printf("tree depth:%d\n", num);

	ret = Root(t1, &node);
	if (ret == OK) {
		printf("have root,root is %d\n", node);
	}else{
		printf("not have root \n");
	}

	return 0;
}
