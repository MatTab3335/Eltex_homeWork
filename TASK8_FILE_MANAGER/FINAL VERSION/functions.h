#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

//--------CURSES-------
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>

void windows_init();
void windows_del();
void windows_resize();
void windows_refresh();
void sig_winch(int signo);
void print(int start);

extern int maxx;
extern int maxy;
extern int key;
extern int highlight;
extern int print_start;

extern WINDOW * name_field;
extern WINDOW * sub_name_field;
extern WINDOW * size_field;
extern WINDOW * sub_size_field;
extern WINDOW * date_field;
extern WINDOW * sub_date_field;

//--------MANAGER-----------
#include <sys/types.h> /* определения типов */
#include <dirent.h>
#include <sys/stat.h>   /* структура, возвращаемая stat */
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>      /* флажки чтения и записи */
#include <malloc.h>

#define MAX_PATH 1024
#define MAX_NAME 512
#define COMMAND_SIZE 10

extern int nfiles;
extern int prev_nfiles;
extern char **name_list;
extern char **path_list;
extern char cur_dir[MAX_PATH];
extern char prev_dir[MAX_PATH];

void allocate();
void free_mem();
void print_names();
void dirwalk(char *dir);
char **resize_list(char **list);
void empty_stdin();


#endif