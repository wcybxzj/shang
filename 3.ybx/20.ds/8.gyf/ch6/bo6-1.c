#define ClearBiTree InitBiTree

void Debug(SqBiTree T)
{
	int i;
	int count = 0;
	for (i = 0; i < MAX_TREE_SIZE; i++) {
		printf("%d:%d ", i, T[i]);
		if (count%9==0) {
			printf("\n");
		}
		count++;
	}
}

Status InitBiTree(SqBiTree T)
{
	int i;
	for (i = 0; i < MAX_TREE_SIZE-1; i++) {
		T[i] = Nil;
	}
	return OK;
}

Status CreateBiTree(SqBiTree T)
{
	int i=0;
	int ret;
#if CHAR
	//省略
#else	
	printf("按照层序请输入节点值 int, 0为空， 999为结束 最多输入%d个节点\n",MAX_TREE_SIZE);
	for (i = 0; i < MAX_TREE_SIZE; i++) {
		ret = scanf("%d", &T[i]);
		if (ret == 0) {
			printf("scanf\n");
			break;
		}
		//结束
		if (T[i]==999) {
			break;
		}
		//此节点不是根,此节点不为空,此节点无双亲节点
		if (i!=0 && T[i]!=Nil &&T[i-1/2]==Nil) {
			printf("出现双亲的非根节点\n");
			return ERROR;
		}
	}

	for (; i < MAX_TREE_SIZE; i++) {
		T[i]= Nil;
	}

#endif
	return OK;
}

Status BiTreeEmpty(SqBiTree T)
{
	if (T[0] == Nil) {
		return TRUE;
	}else{
		return FALSE;
	}
}

int BiTreeDepth(SqBiTree T)
{
	int i, j=0;

	//获取总节点数
	//正着计数有缺陷:
	//例如:11 22 33 0 0 66 999
	//本应算作6 算作3
	//for (i = 0; i < MAX_TREE_SIZE; i++) {
	//	if (T[i] == Nil) {
	//		break;
	//	}
	//}

	//解决方法倒着数
	for (i = MAX_TREE_SIZE-1; i >=0; i--) {
		if (T[i]!=Nil) {
			break;
		}
	}
	i++;

	//i==0//空树
	//i==1//只有根
	//i>1//其他
	//printf("%d\n",i);
	for (j = 0; pow(2, j) <= i; j++);
	return j;
}

Status Root(SqBiTree T, TElemType *nodep)
{
	if (BiTreeEmpty(T)) {
		return ERROR;
	}else{
		*nodep = T[0];
		return OK;
	}
}

//最后一个减一是因为节点从0计数
TElemType Value(SqBiTree T, position e)
{
	return T[(int)pow(2,e.level-1)-1+e.order-1];
}

Status Assign(SqBiTree T, position e, TElemType value)
{
	//找到节点数组索引
	int i = pow(2,e.level-1)-1+e.order-1;
	if (value != Nil && T[i-1/2]== Nil) {
		return ERROR;
	}
	if (value==Nil && (T[2*i+1]!=Nil || T[2*i+2]!=Nil)) {
		return ERROR;
	}
	T[i] = value;
	return OK;
}

TElemType Parent(SqBiTree T, TElemType node)
{
	int i;
	if (T[0]== Nil) {
		return Nil;
	}
	for (i = 1; i < MAX_TREE_SIZE ; i++) {
		if (T[i]==node ){
			return T[(i-1)/2];
		}
	}
	return Nil;
}

TElemType LeftChild(SqBiTree T, TElemType node)
{
	int i;
	if (T[0]==Nil) {
		return Nil;
	}
	for (i = 0; i < MAX_TREE_SIZE ; i++) {
		if (T[i] == node) {
			return T[2*i+1];
		}
	}
	return Nil;
}

TElemType RightChild(SqBiTree T, TElemType node)
{
	int i;
	if (T[0]==Nil) {
		return Nil;
	}
	for (i = 0; i < MAX_TREE_SIZE ; i++) {
		if (T[i] == node) {
			return T[2*i+2];
		}
	}
	return Nil;
}

TElemType LeftSibling(SqBiTree T, TElemType node)
{
	int i;
	if (T[0]==Nil) {
		return Nil;
	}
	//因为root没有左或者右兄弟
	for (i = 1; i < MAX_TREE_SIZE ; i++) {
		if (T[i]==node && i%2==0) {
			return T[i-1];
		}
	}
	return Nil;
}

TElemType RightSibling(SqBiTree T, TElemType node)
{
	int i;
	if (T[0]==Nil) {
		return Nil;
	}
	//因为root没有左或者右兄弟
	for (i = 1; i < MAX_TREE_SIZE ; i++) {
		if (T[i]==node && i%2) {
			return T[i+1];
		}
	}
	return Nil;
}

void Move(SqBiTree t1, int i, SqBiTree t2, int j)
{
	if (t1[2*i+1]!=Nil) {
		Move(t1, 2*i+1, t2, 2*j+1);
	}
	if (t1[2*i+2]!=Nil) {
		Move(t1, 2*i+2, t2, 2*j+2);
	}
	t2[j] = t1[i];
	t1[i] = Nil;
}

//t2插入到t1的值为p的节点
//lr:0在左孩子插入 1在右孩子插入
Status Insert_child(SqBiTree t1, TElemType p, int lr, SqBiTree t2)
{
	int i, j;
	//找到p的位置i
	for (i = 0; i < pow(2, BiTreeDepth(t1))-1; i++) {
		if (t1[i]==p) {
			break;
		}
	}
	//i+lr 获取最后的插入位置
	j = 2*i+1+lr;
	//如果插入位置有节点一律向右移动原有子树
	if (t1[j]!=Nil) {
		Move(t1, j, t1, 2*j+2);
	}
	//t2插入到t1
	Move(t2, 0, t1, j);
	return OK;
}

//删除只能用 前序,中序,后序,不能用层序
void Pre_order_delete(SqBiTree T, int i)
{
	T[i] = Nil;
	if (T[2*i+1]!=Nil) {
		Pre_order_delete(T, 2*i+1);
	}
	if (T[2*i+2]!=Nil) {
		Pre_order_delete(T, 2*i+2);
	}
}

//前提条件:二叉树存在
Status Delete_child1(SqBiTree T, position pos, int lr)
{
	int i;
	i = pow(2, pos.level-1)-1+pos.order-1;
	if (T[i] == Nil) {
		return ERROR;
	}
	i = 2*i+1+lr;
	Pre_order_delete(T, i);
	return OK;
}

Status (*VisitFunc)(TElemType);
void Pre_order_func(SqBiTree T, int i)
{
	VisitFunc(T[i]);
	if (T[2*i+1]!=Nil) {
		Pre_order_func(T, 2*i+1);
	}
	if (T[2*i+2]!=Nil) {
		Pre_order_func(T, 2*i+2);
	}
}

void In_order_func(SqBiTree T, int i)
{
	if (T[2*i+1]!=Nil) {
		In_order_func(T, 2*i+1);
	}
	VisitFunc(T[i]);
	if (T[2*i+2]!=Nil) {
		In_order_func(T, 2*i+2);
	}
}
void Post_order_func(SqBiTree T, int i)
{
	if (T[2*i+1]!=Nil) {
		Post_order_func(T, 2*i+1);
	}
	if (T[2*i+2]!=Nil) {
		Post_order_func(T, 2*i+2);
	}
	VisitFunc(T[i]);
}
void Pre_order(SqBiTree T, Status(* Visit)(TElemType))
{
	VisitFunc = Visit;
	if (!BiTreeEmpty(T)) {
		Pre_order_func(T, 0);
	}
	printf("\n");
}

void In_order(SqBiTree T, Status(* Visit)(TElemType))
{
	VisitFunc = Visit;
	if (!BiTreeEmpty(T)) {
		In_order_func(T, 0);
	}
	printf("\n");
}

void Post_order(SqBiTree T, Status(* Visit)(TElemType))
{
	VisitFunc = Visit;
	if (!BiTreeEmpty(T)) {
		Post_order_func(T, 0);
	}
	printf("\n");
}

void Level_order(SqBiTree T, Status(* Visit)(TElemType))
{
	int i,j;
	//找到节点总数
	//从后往前找
	for(i = MAX_TREE_SIZE-1; i >=0 ; i--) {
		if (T[i]!=Nil) {
			break;
		}
	}
	i++;

	for (j = 0; j < i ; j++) {
		if (T[j]!=Nil) {
			Visit(T[j]);
		}
	}
	printf("\n");
}

//按层打印,每层每个节点从1开始计数
void Print(SqBiTree T)
{
	int i, j;
	int max;
	int node;
	position pos;
	for (i = 1; i <= BiTreeDepth(T); i++) {
		max = pow(2, i-1);
		printf("level:%d\n", i);
		for (j = 1; j <= max; j++) {
			pos.level = i;
			pos.order = j;
			node = Value(T, pos);
			if (node!=Nil) {
				printf("order:%d, value:%d\n", j, node );
			}
		}
		printf("\n");
	}
}
