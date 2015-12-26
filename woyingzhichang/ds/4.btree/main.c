#include <stdio.h>
#include <stdlib.h>

#define NAMESIZE 30

typedef struct _SC {
	int id;
	char name[NAMESIZE];
	int math;
	int chinese;
} SC;

typedef struct _NODE {
	SC data;
	struct _NODE *left, *right;
} NODE;

void myprint(SC *score)
{
	printf("%d %s %d %d\n",
			score->id, score->name, score->math, score->chinese);
}


void draw(NODE *root)
{

}

int insert(NODE **root, SC *data)
{
	NODE *node = NULL;
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

SC *find(NODE *root, int id){
	if (root == NULL) {
		return NULL;
	}
	if (root->data.id == id) {
		return &root->data;
	}
	if (root->data.id >= id) {
		return find(root->left, id);
	}else{
		return find(root->right, id);
	}
}

int main(int argc, const char *argv[])
{
	int arr[] = {1,2,3,7,6,5,9,8,4};
	int i, ret;
	SC tmp, *sc_p;
	NODE *tree = NULL;

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		tmp.id = arr[i];
		snprintf(tmp.name, NAMESIZE, "stu%d", arr[i]);
		tmp.math = rand()%100;
		tmp.chinese = rand()%100;
		insert(&tree, &tmp);
	}

	int tmp_id = 2;
	sc_p= find(tree, tmp_id);
	if (sc_p) {
		printf("find:\n");
		myprint(sc_p);
	}else{
		printf("not find\n");
	}

	tmp_id =100;
	sc_p= find(tree, tmp_id);
	if (sc_p) {
		printf("find:");
		myprint(sc_p);
	}else{
		printf("not find\n");
	}




	return 0;
}
