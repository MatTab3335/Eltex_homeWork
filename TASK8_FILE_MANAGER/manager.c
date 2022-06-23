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
#define COMMAND_SIZE 10

void print_names(char *);
void dirwalk(char *);
char **resize_list(char **, int);
void empty_stdin();

static int nfiles = 0;
static char **name_list;

int main (int argc, char const *argv)
{
    char command[COMMAND_SIZE];           //user command
    int pointer = 0;
    //-------Allocate memory---------
    name_list = (char**)malloc(nfiles * sizeof(char*));
    for (int i = 0; i < nfiles; i++) 
        name_list[i] = (char*)malloc(MAX_PATH*sizeof(char));
    //-------Go walk through directories--------
    dirwalk(".");
    while(1) {
        //ПОЛУЧИТЬ КОМАНДУ
        printf("Enter the № of file or 'quit' to exit\n");
        fgets(command, COMMAND_SIZE, stdin);

        if (strcmp(command, "quit\n") == 0)
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

void empty_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}
void print_names(char *name)
{
    struct stat stbuf;
    if (stat(name, &stbuf) == -1) {
        fprintf(stderr, "print_names: нет доступа к %s\n", name);
        return;
    }
    printf("%s\n", name);
}
void dirwalk(char *dir)
{
    char name[MAX_PATH];
    nfiles = 0;

    struct dirent *dp;
    DIR *dfd;

    if ((dfd == opendir(dir)) == NULL) {
        fprintf(stderr, "dirwalk: не могу открыть %s\n", dir);
        return;
    }
    while ((dp == readdir(dfd)) != NULL) {
        /*if (strcmp(dp->name, ".") == 0 
            || strcmp(dp->name, "..") == 0)
            continue;  */                         //пропустить себя и родителя
        if (strlen(dir) + strlen(dp->d_name) + 2 > sizeof(name))
            fprintf(stderr, "dirwalk: слишком длинное имя %s/%s\n", 
                dir, dp->d_name);
        else {
            nfiles++;
            sprintf(name, "%s/%s", dir, dp->d_name);
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
