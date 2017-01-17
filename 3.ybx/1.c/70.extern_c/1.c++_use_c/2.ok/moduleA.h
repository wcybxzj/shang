#ifndef MODULE_A_H
#define MODULE_A_H

//第1种extern "C"写法
#ifdef __cplusplus
extern "C" {
#endif

void foo( int x, int y );

#ifdef __cplusplus
}
#endif

#endif
