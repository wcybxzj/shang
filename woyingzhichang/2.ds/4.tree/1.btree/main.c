#include <stdio.h>
#include <stdlib.h>

#include <llist.h>
#include <queue.h>

#define NAMESIZE 30

typedef struct _SC {
	int id;
	char name[NAMESIZE];
	int math;
	int chinese;
} SC;

typedef struct _BT_NODE {
	SC data;
	struct _BT_NODE *left, *right;
} BT_NODE;

static BT_NODE *tree = NULL;

void myprint(SC *score)
{
	printf("%d %s %d %d\n",
			score->id, score->name, score->math, score->chinese);
}

void draw_(BT_NODE *root , int level)
{
	int i;
	if (NULL == root) {
		return;
	}
	draw_(root->right, level+1);
	for (i = 0; i < level; i++) {
		printf("	");
	}
	//sleep(1);
	myprint(&root->data);
	draw_(root->left, level+1);
}

void draw(BT_NODE *root)
{
	draw_(root, 0);
}

void draw_debug(BT_NODE *root)
{

	printf("\n\n plz enter something !\n");
	getchar();
	draw_(root, 0);
}

int insert(BT_NODE **root, SC *data)
{
	BT_NODE *node = NULL;
	if (NULL == *root) {
		node = malloc(sizeof(*node));
		if (NULL == node) {
			return -1;
		}
		node->data = *data;
		node->left = node->right = NULL;
		*root = node;
		return 0;
	}

	if ((*root)->data.id >= data->id) {
		insert(&(*root)->left, data);
	}else{
		insert(&(*root)->right, data);
	}
	return 0;
}

BT_NODE *find(BT_NODE *root, int id)
{
	if (root == NULL) {
		return NULL;
	}
	if (root->data.id == id) {
		return root;
	}
	if (root->data.id >= id) {
		return find(root->left, id);
	}else{
		return find(root->right, id);
	}
}

static int get_num(BT_NODE *root)
{
	int ret = 0;
	if (NULL == root) {
		return ret;
	}
	ret += get_num(root->left);
	ret += 1;
	ret += get_num(root->right);
	return ret;
}

//递归版
static BT_NODE *find_min(BT_NODE *root){
	if (root->left == NULL) {
		return root;
	}
	return find_min(root->left);
}

//static BT_NODE *find_min(BT_NODE *root){
//	while (root->left) {
//		root = root->left;
//	}
//	return root;
//}

static BT_NODE *find_max(BT_NODE *root){
	while (root->right) {
		root = root->right;
	}
	return root;
}

static void turn_left(BT_NODE **root)
{
	BT_NODE *tmp_root = *root;
	*root = (*root)->right;
	tmp_root->right = NULL;
	find_min(*root)->left = tmp_root;
	//draw_debug(tree);
}

static void turn_right(BT_NODE **root)
{
	BT_NODE *tmp_root = *root;
	*root = (*root)->left;
	tmp_root->left= NULL;
	find_max(*root)->right= tmp_root;
	//draw_debug(tree);
}

void balance(BT_NODE **root)
{
	int sub;
	if (NULL == *root) {
		return;
	}
	while (1) {
		sub = get_num((*root)->left) - get_num((*root)->right);
		if (sub >= -1 && sub <= 1) {
			break;
		}
		if (sub < -1) {
			turn_left(root);
		}else{
			turn_right(root);
		}
	}

	balance(&(*root)->right);
}

//极端情况会删除总的根节点所以需要2级指针
void delete(BT_NODE **root, int id){
	BT_NODE **node = root, *cur = NULL;
	while (*node != NULL && (*node)->data.id != id) {
		if ((*node)->data.id > id  ) {
			node = &(*node)->left;
		}else{
			node = &(*node)->right;
		}
	}

	if (NULL == *node) {
		return;
	}

	//假设优先左树
	cur = *node;
	if (cur->left == NULL) {
		*node = (*node)->right;
	}else{
		*node = (*node)->left;
		find_max(*node)->right = cur->right;
	}
	free(cur);
}

//先
void travel_xian(BT_NODE *root)
{
	if (root == NULL) {
		return;
	}
	myprint(&root->data);
	travel_xian(root->left);
	travel_xian(root->right);
}

//中
void travel_zhong(BT_NODE *root)
{
	if (root == NULL) {
		return;
	}
	travel_zhong(root->left);
	myprint(&root->data);
	travel_zhong(root->right);
}

//后
void travel_hou(BT_NODE *root)
{
	if (root == NULL) {
		return;
	}
	travel_hou(root->left);
	travel_hou(root->right);
	myprint(&root->data);
}

//层式遍历
void travel_level(BT_NODE *root)
{
	int ret;
	BT_NODE *cur = NULL;
	Q* me = NULL;
	me = q_create(sizeof(BT_NODE *));
	if (NULL == me) {
		return;
	}
	q_enqueue(me, &root);
	while (1) {
		ret = q_dequeue(me, &cur);
		if (ret == 0) {
			break;
		}
		myprint(&cur->data);
		if (cur->left) {
			q_enqueue(me, &(cur->left));
		}

		if (cur->right) {
			q_enqueue(me, &(cur->right));
		}
	}
	q_destroy(me);
}

int main(int argc, const char *argv[])
{
	int arr[] = {1,2,3,7,6,5,9,8,4};
	int i;
	SC tmp;
	BT_NODE *tmp_node;

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		tmp.id = arr[i];
		snprintf(tmp.name, NAMESIZE, "stu%d", arr[i]);
		tmp.math = rand()%100;
		tmp.chinese = rand()%100;
		insert(&tree, &tmp);
	}

	int tmp_id = 2;
	tmp_node = find(tree, tmp_id);
	if (tmp_node) {
		printf("find:\n");
		myprint(&tmp_node->data);
	}else{
		printf("not find\n");
	}

	tmp_id =100;
	tmp_node= find(tree, tmp_id);
	if (tmp_node) {
		printf("find:");
		myprint(&tmp_node->data);
	}else{
		printf("not find\n");
	}

	printf("===========draw============\n");
	draw(tree);

	printf("===========balancing============\n");
	balance(&tree);
	draw(tree);

	printf("===========delete============\n");
	delete(&tree, 5);
	//delete(&tree, 1);
	//delete(&tree, 7);
	draw(tree);

	printf("=============qian===================\n");
	travel_xian(tree);
	printf("=============zhong===================\n");
	travel_zhong(tree);
	printf("=============hou===================\n");
	travel_hou(tree);
	printf("=============level===================\n");
	travel_level(tree);

	return 0;
}
