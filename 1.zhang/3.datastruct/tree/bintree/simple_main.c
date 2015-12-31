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
	struct node_st *newnode = NULL;
	struct node_st **ret = root;

	newnode = malloc(sizeof(*newnode));
	if (NULL == newnode)
		return -1;	
	newnode->data = *data;
	newnode->left = newnode->right = NULL;

	while (*ret != NULL) {
		/* code */
		if ((*ret)->data.id >= data->id) {
			ret = &(*ret)->left;
		} else {
			ret = &(*ret)->right;
		}
	}
	*ret = newnode;

	return 0;
}

struct data_st *search_tree(struct node_st *root, int *id)
{
	struct node_st *p = root;

	while (p != NULL) {
		if (p->data.id == *id) {
			return &p->data;
		}	
		if (p->data.id < *id) {
			p = p->right;
		} else {
			p = p->left;
		}
	}

	return NULL;
}

int main(int argc, const char *argv[])
{
	int arr[] = {3,2,7,1,6,8,9,4};
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
	
	return 0;
}
