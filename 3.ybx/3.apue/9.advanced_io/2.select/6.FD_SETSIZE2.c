#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#define NUM 2000
int fds[NUM];



/*
[0]:3
[1]:4
[2]:5
[3]:6
[4]:7
[5]:8
[6]:9
[7]:10
[8]:11
[9]:12
.....
.....
.....
[1054]:1057
[1055]:1058
[1056]:1059
[1065]:1060 <---开始出错
[1082]:1061
[1083]:1062
[1148]:1063
[1277]:1064
[1534]:1065 <--没跑到[1999] 也没fail 
succ 
*/

//原因:FD_SET的第一个参数不能超过FD_SETSIZE（在<bits/typesizes.h>里定义

int main()
{
	int i;
    for( i = 0; i < NUM; ++i )
    {   
        printf("[%d]:", i); 
        if( ( fds[i] = socket( AF_INET, SOCK_STREAM, 0 ) ) >= 0 ) 
        {   
            printf( "%d\n",fds[i] );
            fd_set rset;
            FD_ZERO(&rset);
            FD_SET( fds[i], &rset);
        }   
        else
        {   
            printf( "fail\n" );
        }   
    }   
    printf( "succ\n" );
    return 0;
}
