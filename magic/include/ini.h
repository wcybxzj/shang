#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>


//-----------------------
#include "type.h"
#include "mypthread.h"



//-----------------------
void init();
int check_can_change(int,int,int);
int make_rnd();
void box(unsigned short *,int,int,unsigned short);
void set_cursor_raw(struct termios *);
//-----------test------------
//void box(unsigned short *,int,int);
