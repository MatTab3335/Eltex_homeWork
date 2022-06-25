#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdio.h>
#include <sys/types.h> /* определения типов */
#include <dirent.h>
#include <sys/stat.h>   /* структура, возвращаемая stat */
#include <string.h>
#include <stdlib.h>
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

void allocate();
void print_names(char *name);
void dirwalk(char *dir);
char **resize_list(char **list);
void empty_stdin();

#endif