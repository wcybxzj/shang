#include <stdio.h>
#include <stdlib.h>

typedef struct ploy_st {
	int coef;//系数
	int exp;//指数
	struct ploy_st *next;
} NODE;

//int arr[][3] 实际是指针, sizeof(arr) 4字节
NODE * poly_creat(int arr[][2], int one_num){
	int i;
	NODE *head_p = NULL, *tmp_head_p = NULL,*tmp_next = NULL;
	head_p = malloc(sizeof(NODE));
	head_p->next = NULL;
	tmp_head_p = head_p;
	for (i = 0; i < one_num; i++) {
		tmp_next = malloc(sizeof(NODE));
		tmp_next->coef = arr[i][0];
		tmp_next->exp = arr[i][1];
		tmp_head_p->next = tmp_next;
		tmp_head_p = tmp_next;
	}
	return head_p;
}

void ploy_show(NODE *ptr){
	NODE *cur= NULL;
	for (cur = ptr->next; cur!=NULL; cur = cur->next) {
		printf("(coef:%d ", cur->coef);
		printf("exp:%d) ", cur->exp);
	}
	printf("\n");
}

//lp是最后那个节点，通过判断决定下一个节点
int ploy_union(NODE *des_p, NODE *src_p){
	NODE *dp = des_p->next, *sp = src_p->next, *lp = des_p;
	while (dp && sp){
		if (dp->exp < sp->exp){
			lp->next = dp;
			lp = dp;
			dp = dp->next;
		}else if (dp->exp > sp->exp){
			lp->next = sp;
			lp = sp;
			sp = sp->next;
		}else{
			dp->coef += sp->coef;
			if (dp->coef){
				lp->next = dp;
				lp = dp;
			}
			dp = dp->next;
			sp = sp->next;
		}
	}
	if (dp == NULL) {
		lp->next= sp;
	}else if(sp == NULL){
		lp->next = dp;
	}
}

int my_ploy_union(NODE *des_p, NODE *src_p){
	int tmp_coef = 0;
	NODE *cur_des = NULL, *cur_src = NULL;
	NODE *prev_cur_des = des_p;

	for (cur_des = des_p->next; cur_des != NULL; \
			prev_cur_des = cur_des, cur_des = cur_des->next) {
		for (cur_src = src_p->next; cur_src != NULL;\
				cur_src = cur_src->next) {
			if (cur_des->exp == cur_src->exp) {
				if ((tmp_coef = cur_des->coef + cur_src->coef) == 0) {
					prev_cur_des->next = cur_des->next;
					free(cur_des);
				}else{
					cur_des->coef = tmp_coef;
				}
			}
		}
	}
}

int main(int argc, const char *argv[])
{
	int i, j;
	NODE *p = NULL, *p1 = NULL;
	int arr[][2] = {{5, 0}, {2, 1}, {8, 8}, {3, 16}};
	int arr1[][2] = {{6, 1}, {16, 6}, {-8, 8}};

#if 0
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		for (j = 0; j <sizeof(arr[i])/sizeof(*arr[i]) ; j++) {
			printf("%d\n", arr[i][j]);
		}
		printf("\n");
	}
#endif

	p = poly_creat(arr, sizeof(arr)/sizeof(*arr));
	p1 = poly_creat(arr1, sizeof(arr1)/sizeof(*arr1));
	ploy_show(p);
	ploy_show(p1);

	ploy_union(p, p1);
	ploy_show(p);

	return 0;
}
