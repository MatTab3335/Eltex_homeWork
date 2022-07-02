#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

//--------CURSES-------
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>

void windows_init();		//инициализация окон
void windows_del();			//удаление окон
void windows_resize();		//обновление размеров окон
void windows_refresh();		//обновление содержимого окон
void sig_winch(int signo);	//обработка сигнала по изм-ию размера
void print(int start);		//вывод содержимого массивов в окно

extern int maxx;			//максимальный размер окна по x
extern int maxy;			//макс. размер окна по y
extern int key;				//нажатая клавиша
extern int highlight;		//переменная для подсветки тек. позиции в списке
extern int print_start;		//перем. для прокрутки экрана

extern WINDOW * name_field;		//основное окно
extern WINDOW * sub_name_field;	//внутреннее окно для отображения текста

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

extern int nfiles;          //кол-во файлов в директории
extern int prev_nfiles;		//кол-во файлов в пред.директории
extern char **name_list;	//массив имен каталога
extern char **path_list;	//массив полных адресов каталога
extern char cur_dir[MAX_PATH];	//тек. директория
extern char prev_dir[MAX_PATH]; //пред. директория

void allocate();		//ф-ция для выделения динамических массивов имен и адресов
void free_mem();		//ф-ция для освобождения памяти
void dirwalk(char *dir);//ф-ция для захода в каталог и сохранения в массивах имен и адресов
char **resize_list(char **list);	//ф-ция для изм-ия размера массивов
void empty_stdin();					//очистка входного буфера


#endif