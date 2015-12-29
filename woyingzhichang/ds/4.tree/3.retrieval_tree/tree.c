#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 512
#define KEY_SIZE 256
#define DESC_SIZE 256

#define FNAME "log"

struct NODE {
	struct NODE *ch[26];
	char desc[DESC_SIZE];
};

int get_word(FILE *fp, char *key, char *desc)
{
	char buf[BUFSIZE];
	if (NULL == fgets(buf, BUFSIZE, fp)) {
		return -1;
	}

	char *tokenremains = buf;
	char *tmp_key = strsep(&tokenremains, ": ");
	strsep(&tokenremains, " ");
	char *tmp_desc = strsep(&tokenremains, "\n");

	strcpy(key, tmp_key);
	strcpy(desc, tmp_desc);
	//printf("%d\n", strlen(tokenremains));
	return 0;
}


struct NODE *find(struct NODE *root, char *key){
	if (root == NULL) {
		return NULL;
	}
	if (*key == '\0') {
		return root;
	}
	return find(root->ch[*key-'a'], key+1);
}

struct NODE * newnode(void){
	int i;
	struct NODE *node;
	node = malloc(sizeof(*node));
	if (NULL == node) {
		return NULL;
	}
	node->desc[0] = '\0';
	//memset(node->desc, '\0', DESC_SIZE);
	for (i = 0; i < 26; i++) {
		node->ch[i] = NULL;
	}
	return node;
}

//巧妙
int insert(struct NODE **root, char *key, char *desc){
	int i;
	if (NULL == *root) {
		*root = newnode();
		if (NULL == *root) {
			return -1;
		}
	}
	if (*key == '\0') {
		strcpy((*root)->desc, desc);
		return 0;
	}
	//return insert((*root)->ch+*key-'a', key+1, desc);
	return insert(&((*root)->ch[*key-'a']), key+1, desc);
}

int main(int argc, const char *argv[])
{
	struct NODE *tree=NULL, *tmp=NULL;
	int ret;
	FILE *fp;
	char key[KEY_SIZE] = {'\0'};
	char desc[DESC_SIZE] = {'\0'};
	fp = fopen(FNAME, "r");
	if (NULL == fp) {
		fprintf(stderr, "fopen() error\n");
		exit(1);
	}

	while (1) {
		ret = get_word(fp, key, desc);
		if (ret < 0) {
			break;
		}

		//printf("%s ", key);
		//printf("%s\n", desc);
		//printf("sizeof %d\n", strlen(key));
		//printf("sizeof %d\n", strlen(desc));
		insert(&tree, key, desc);
	}

	tmp = find(tree, "donkey");
	printf("%s\n", tmp->desc);

	fclose(fp);
	return 0;
}
