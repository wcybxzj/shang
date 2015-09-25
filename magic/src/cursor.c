#include "../include/ini.h"
void set_cursor_raw(struct termios *old){
	struct termios cursor_raw;
	tcgetattr(0,old);
	cfmakeraw(&cursor_raw);
	tcsetattr(0,TCSANOW,&cursor_raw);

	return ;
}
