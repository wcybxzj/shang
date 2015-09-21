#include <stdio.h>
#include <stdlib.h>

#define NAMESIZE 32

struct data_st {
	int id;
	char name[NAMESIZE];
} /* optional variable list */;

struct node_st {
	struct node_st *left;
	struct node_st *right;
	struct data_st data;
} /* optional variable list */;

int insert_tree(struct node_st **root, struct data_st *data)
{
	if (*root == NULL) {
		struct node_st *newnode = NULL;
		
		newnode = malloc(sizeof(*newnode));
		if (NULL == newnode) 
			return -1;
		newnode->data = *data;
		newnode->left = newnode->right = NULL;
		
		*root = newnode;	
		return 0;
	}
	if (data->id >= (*root)->data.id) {
		return insert_tree(&(*root)->right, data);
	} else {
		return insert_tree(&(*root)->left, data);
	}

	return 0;
}

struct data_st *search_tree(struct node_st *root, int *id)
{
	if (root == NULL)
		return NULL;
	if (root->data.id == *id)
		return &root->data;		
	if (root->data.id > *id) 
		return search_tree(root->left, id);
	else {
		return search_tree(root->right, id);
	}
}

void traval_pre(struct node_st *root)
{
	if (root == NULL)
		return ;
	printf("%d ", root->data.id);
	traval_pre(root->left);
	traval_pre(root->right);
}

void draw__(struct node_st *root, int level)
{
	int i;

	if (root == NULL)
		return ;

	draw__(root->right, level + 1);
	for (i = 0; i < level; i++) {
		printf("     ");
	}
	printf("%d %s\n", root->data.id, root->data.name);
	draw__(root->left, level + 1);
}

void draw(struct node_st *root)
{
	draw__(root, 0);	
}

static struct node_st *find_min(struct node_st *root)
{
	if (root->left == NULL) {
		return root;
	}	
	find_min(root->left);
}

static struct node_st *find_max(struct node_st *root)
{
	if (root->right == NULL)
		return root;
	find_max(root->right);
}

static void delete__(struct node_st **root)
{
	struct node_st *cur, *l, *r;

	cur = *root;
	l = cur->left;
	r = cur->right;

	if (r == NULL) {
		*root = l;
		cur->left = NULL;
		free(cur);
		return ;
	} else {
		find_min(r)->left = l;
		*root = r;
		cur->right = NULL;
		free(cur);
		return;
	}
}

int delete_tree(struct node_st **root, int *id)
{
	if (*root == NULL)
		return -1;

	if ((*root)->data.id == *id) {
		delete__(root);
		return 0;
	}	
	if ((*root)->data.id < *id) {
		delete_tree(&(*root)->right, id);
	} else {
		delete_tree(&(*root)->left, id);
	}
}

static int get_num(struct node_st *root)
{
	if (root == NULL) {
		return 0;
	}	

	return get_num(root->left) + get_num(root->right) + 1;
}

static void turn_right(struct node_st **root)
{
	struct node_st *cur, *l, *r;

	cur = *root;
	l = cur->left;
	r = cur->right;	

	*root = l;
	cur->left = NULL;
	find_max(l)->right = cur;

	getchar();
	draw(*root);
}

static void turn_left(struct node_st **root)
{
	struct node_st *cur, *l, *r;

	cur = *root;
	l = cur->left;
	r = cur->right;	

	*root = r;
	cur->right = NULL;
	find_min(r)->left = cur;	
	
	getchar();
	draw(*root);
}

void balance_tree(struct node_st **root)
{
	int num;
	if (*root == NULL)
		return ;		

	while (1) {
		num = get_num((*root)->left)-get_num((*root)->right);
		if (num > 1) {
			turn_right(root);	
		} else if (num < -1) {
			turn_left(root);
		} else 
			break;
	}

	balance_tree(&(*root)->left);
	balance_tree(&(*root)->right);
}

int main(int argc, const char *argv[])
{
	int arr[] = {1,2,3,4,5,6,7,8,9,10};
	int i, id;
	struct node_st *tree = NULL;
	struct data_st tmp;
	struct data_st *ret;

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		tmp.id = arr[i];
		snprintf(tmp.name, NAMESIZE, "stu%d", arr[i]);

		insert_tree(&tree, &tmp);
	}

	id = 6;
	ret = search_tree(tree, &id);
	if (ret != NULL) {
		printf("find it: %d %s\n", ret->id, ret->name);
	} else {
		printf("not found\n");
	}

	traval_pre(tree);
	printf("\n");

	draw(tree);

	balance_tree(&tree);
	draw(tree);

#if 0
	printf("\n**********delete***********\n");
	delete_tree(&tree, &id);
	draw(tree);

	id = 9;
	printf("\n**********delete***********\n");
	delete_tree(&tree, &id);
	draw(tree);
#endif
	
	return 0;
}
