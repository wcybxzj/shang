#include <stdio.h>
union data{
    int n;
    char ch;
    short m;
};

int main(){
    union data a;
    printf("%d, %d\n", sizeof(a), sizeof(union data) );//8, 8
    a.n = 0x40;
    printf("%X, %c, %hX\n", a.n, a.ch, a.m);//40, @, 40
    a.ch = '9';
    printf("%X, %c, %hX\n", a.n, a.ch, a.m);//39, 9, 39
    a.m = 0x2059;
    printf("%X, %c, %hX\n", a.n, a.ch, a.m);//2059, Y, 2059
    a.n = 0x3E25AD54;
    printf("%X, %c, %hX\n", a.n, a.ch, a.m);//3E25AD54, T, AD54
    return 0;
}
