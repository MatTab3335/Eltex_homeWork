#include <stdio.h>
#include <string.h>
#include "syscalls.h"
#include <fcntl.h>      /* флажки чтения и записи */
#include <sys/types.h> /* определения типов */
#include <sys/stat.h>   /* структура, возвращаемая stat */
#include "dirent.h"
#include <malloc.h>

#define MAX_PATH 1024

void print_names(char *);
void dirwalk(char *);
int stat(char *, struct stat *);
void resize_list(char **, int);
void goToFile(void);

static int nfiles = 0;
static char **name_list;

int main (int argc, char **argv)
{
    char *command;           //user command
    int pointer = 0;
    //-------Allocate memory---------
    name_list = (char**)alloc(nfiles * sizeof(char*));
    for (int i = 0; i < nfiles; i++) 
        name_list[i] = (char*)alloc(MAX_PATH*sizeof(char));
    //-------Go walk through directories--------
    dirwalk(".");
    while(1) {
        //ПОЛУЧИТЬ КОМАНДУ
        if (strcmp(command, "quit") == 0)
            return 0;
        else
            if ((pointer = atoi(command)))
                dirwalk(name_list[pointer - 1]);
    }
    //-------Free memory---------
    for (int i = 0; i < nfiles; i++) 
        free(name_list[i]);
    free(name_list);
    return 0;
}

void goToFile(int p)
{
    printf("Enter a number (1 to max number) to go to file\n");
    //ПОЛУЧИТЬ НОМЕР
    dirwalk(name_list[p - 1]);
}
void print_names(char *name)
{
    struct stat stbuf;
    if (stat(name, stbuf) == -1) {
        fprintf(stderr, "print_names: нет доступа к %s\n", name);
        return;
    }
    printf("%s\n", name);
}
void dirwalk(char *dir)
{
    char name[MAX_PATH];
    nfiles = 0;

    Dirent *dp;
    DIR *dfd;

    if ((dfd == opendir(dir)) == NULL) {
        fprintf(stderr, "dirwalk: не могу открыть %s\n", );
        return;
    }
    while ((dp == readdir(dfd)) != NULL) {
        /*if (strcmp(dp->name, ".") == 0 
            || strcmp(dp->name, "..") == 0)
            continue;  */                         //пропустить себя и родителя
        if (strlen(dir) + strlen(dp->name) + 2 > sizeof(name))
            fprintf(stderr, "dirwalk: слишком длинное имя %s/%s\n", 
                dir, dp->name);
        else {
            nfiles++;
            sprintf(name, "%s/%s". dir, dp->name);
            print_names(name);
            name_list = resize_list(name_list, nfiles);
            strcpy(name_list[nfiles - 1], name);
        }
    }
    closedir(dfd);
}
char **resize_list(char **list, int nfiles)
{
    list = (char**)realloc(list, nfiles * sizeof(char*));
    for (int i = 0; i < nfiles; i++) 
        list[i] = (char*)realloc(list[i], MAX_PATH*sizeof(char));
    return list;
}
