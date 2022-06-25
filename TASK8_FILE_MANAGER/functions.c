#include "functions.h"

int nfiles = 0;
int prev_nfiles = 0;
char **name_list;
char **path_list;

void allocate() 
{
    //-------Allocate memory---------
    if (!(name_list = (char**)malloc(nfiles * sizeof(char*)))) {
        fprintf(stderr, "Allocation error! name_list\n");
        exit(1);
    }
    if (!(path_list = (char**)malloc(nfiles * sizeof(char*)))) {
        fprintf(stderr, "Allocation error! name_list\n");
        exit(1);
    }
    for (int i = 0; i < nfiles; i++) 
        if (!(name_list[i] = (char*)malloc(MAX_PATH*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list[i]\n");
            exit(1);
        }
    for (int i = 0; i < nfiles; i++) 
        if (!(name_list[i] = (char*)malloc(MAX_NAME*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list[i]\n");
            exit(1);
        }
}
void empty_stdin() 
{
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
//	printf("Im in dirwalk\n");
    char path[MAX_PATH];
    char name[MAX_NAME];
    nfiles = 0;
    struct dirent *dp;
    DIR *dfd;

    if ((dfd = opendir(dir)) == NULL) {
    	printf("dirwalk: не могу открыть %s\n", dir);
        // fprintf(stderr, "dirwalk: не могу открыть %s\n", dir);
        return;
    }
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 )
            continue;                          //пропустить себя и родителя
        if (strlen(dir) + strlen(dp->d_name) + 2 > sizeof(path))
            fprintf(stderr, "dirwalk: слишком длинное имя %s/%s\n", 
                dir, dp->d_name);
        else {
            nfiles++;
            sprintf(path, "%s/%s", dir, dp->d_name);
            strcpy(name, dp->d_name);
            printf("%s\n", name);
            name_list = resize_list(name_list);
            strcpy(name_list[nfiles - 1], name);
            path_list = resize_list(path_list);
            strcpy(path_list[nfiles - 1], path);
            prev_nfiles = nfiles;
        }
    }
   	closedir(dfd);
}
char **resize_list(char **list)
{
    if (!(list = (char**)realloc(list, nfiles * sizeof(char*)))) {
        fprintf(stderr, "Reallocation error! name_list\n");
        exit(1);
    }
    if (prev_nfiles < nfiles)
        for (int i = prev_nfiles; i < nfiles; i++) 
        if (!(list[i] = (char*)malloc(MAX_PATH*sizeof(char)))) {
            fprintf(stderr, "Allocation error! name_list\n");
            exit(1);
        }
    else if (prev_nfiles > nfiles) {
        for (int i = nfiles; i < prev_nfiles; i++) 
            free(name_list[i]);
    }
    return list;  
}