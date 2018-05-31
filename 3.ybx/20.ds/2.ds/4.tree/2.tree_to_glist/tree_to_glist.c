#include <stdio.h>
#include <stdlib.h>

#define FNAME "/tmp/out"

typedef struct _BT_NODE {
	int data;
	struct _BT_NODE *left, *right;
} BT_NODE;


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
	printf("%c\n ", root->data);
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

//char 一般都定义成int
int insert(BT_NODE **root, int data)
{
	BT_NODE *node = NULL;
	if (NULL == *root) {
		node = malloc(sizeof(*node));
		if (NULL == node) {
			return -1;
		}
		node->data = data;
		node->left = node->right = NULL;
		*root = node;
		return 0;
	}

	if ((*root)->data >= data) {
		insert(&(*root)->left, data);
	}else{
		insert(&(*root)->right, data);
	}
	return 0;
}

void save_(BT_NODE *root, FILE *fp)
{
	fputc('(', fp);
	if (NULL == root) {
		fputc(')', fp);
		return;
	}
	fputc(root->data, fp);
	save_(root->left, fp);
	save_(root->right, fp);
	fputc(')', fp);
}

//树转广义表
int save(BT_NODE *root, const char *fname)
{
	FILE *fp;
	fp = fopen(fname, "w");
	if (NULL == fp) {
		return -1;
	}
	save_(root, fp);
	fclose(fp);
	return 0;
}

BT_NODE *load_(FILE *fp)
{
	BT_NODE *root = NULL;
	int c;
	c = fgetc(fp);
	if (c != '(') {
		fprintf(stderr, "format error\n");
		return NULL;
	}
	c = fgetc(fp);
	if (c==')') {
		return NULL;
	}

	root = malloc(sizeof(*root));
	if (NULL == root) {
		return NULL;
	}
	root->data = c;
	root->left = load_(fp);
	root->right = load_(fp);
	c = fgetc(fp);//仍然是一个 )
	if (c != ')') {
		fprintf(stderr, "format error\n");
		return NULL;
	}
	return root;
}

BT_NODE *load(const char *fname)
{
	FILE *fp;
	BT_NODE *root;
	fp = fopen(fname, "r");
	if (NULL == fp) {
		return NULL;
	}
	root = load_(fp);
	fclose(fp);
	return  root;
}

int main(int argc, const char *argv[])
{
	BT_NODE *tree = NULL;
	char arr[] = "cefadjbh";
	int i;

	for (i = 0; i < sizeof(arr)/sizeof(*arr)-1; i++) {
		insert(&tree, arr[i]);
	}

	printf("================save======================\n");
	draw(tree);
	save(tree, FNAME);

	printf("================load======================\n");

	tree = NULL;
	tree = load(FNAME);
	draw(tree);

	return 0;
}
