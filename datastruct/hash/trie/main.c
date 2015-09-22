#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DICNR 26
#define WORDSIZE 1024
#define DSCPSIZE 1024
#define BUFSIZE  1024

struct node_st {
	struct node_st *arr[DICNR];
	char *descr;
};

static int get_string(FILE *fp, char *word, char *dscp)
{
	char buf[BUFSIZE] = {};
	
	fgets(buf, BUFSIZE, fp);
	if (buf[0] == '\n')
		return 1;
	if (buf[0] == '\0') 
		return -1;
	
	sscanf(buf, "%s %s", word, dscp);
	return 0;
}

static struct node_st *malloc_space(void)
{
	struct node_st *new = NULL;
	int i;

	new = malloc(sizeof(*new));
	if (NULL == new)
		return NULL;
		
	for (i = 0; i < DICNR; i++) {
		(new->arr)[i] = NULL;
	}
	new->descr = NULL;

	return new;
}

static int insert_tree(struct node_st **root, char *word, char *desc)
{
	if (*root == NULL) {
		*root = malloc_space();
	}	
	if (*word == '\0') {
		(*root)->descr = strdup(desc);
		return 0;
	}
	insert_tree(&(*root)->arr[*word-'a'], word + 1, desc);
}

static const char *search_tree(struct node_st *root, const char *ptr)
{
	if (root == NULL)
		return NULL;
	if (*ptr == '\0') {
		return root->descr;
	}	

	search_tree(root->arr[*ptr-'a'], ptr+1);
}

int main(int argc, const char *argv[])
{
	struct node_st *tree = NULL;	
	char word[WORDSIZE] = {};
	char dscp[DSCPSIZE] = {};
	FILE *fp = NULL;
	int ret;
	const char *p = NULL;
	
	fp = fopen("dictionary.txt", "r");
	if (fp == NULL)
		exit(1);

	while (1) {
		ret = get_string(fp, word, dscp);	
		if (ret < 0) 
			break;
		if (ret > 0)
			continue;

		insert_tree(&tree, word, dscp);	
	}	

	p = search_tree(tree, "zhangyanbing");
	if(NULL == p) {
		printf("not have\n");
	} else {
		printf("%s\n", p);
	}

	fclose(fp);
	return 0;
}
