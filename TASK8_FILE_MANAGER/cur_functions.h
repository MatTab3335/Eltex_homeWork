#ifndef _CUR_FUNCTIONS_H_
#define _CUR_FUNCTIONS_H_

#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

void windows_init();
void windows_del();
void windows_resize();
void windows_refresh();
void sig_winch(int signo);

extern int maxx;
extern int maxy;
extern int flag_refresh;

extern WINDOW * name_field;
extern WINDOW * sub_name_field;
extern WINDOW * size_field;
extern WINDOW * sub_size_field;
extern WINDOW * date_field;
extern WINDOW * sub_date_field;

#endif