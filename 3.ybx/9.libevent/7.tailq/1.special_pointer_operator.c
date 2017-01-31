#include<stdio.h>
//帮助理解TAILQ_LAST 和 TAILQ_PREV
struct item_t
{
    int aa;
    int bb;
    int cc;
};

struct entry_t
{
    int a;
    int b;
};


int main()
{
    struct item_t item = {1, 2, 3};

    struct entry_t *p = (struct entry_t*)(&item.bb);
    printf("a = %d, b = %d\n", p->a, p->b);

    return 0;
}

