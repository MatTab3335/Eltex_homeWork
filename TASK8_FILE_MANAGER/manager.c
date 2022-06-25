#include "functions.h"

int main (int argc, char const *argv)
{
    char command[COMMAND_SIZE];           //user command
    int pointer = 0;
    char cur_dir[MAX_PATH];
    char prev_dir[MAX_PATH];
    
    //-------Go walk through directories--------
    strcpy(cur_dir, "/home/matvey");
    dirwalk(cur_dir);
    while(1) {
        //ПОЛУЧИТЬ КОМАНДУ
        /*puts("\nArray: ");
        for (int i = 0; i < nfiles; i++) {
            puts(path_list[i]);
        }*/
        printf("Enter the № of file or 'quit' to exit\n");
        fgets(command, COMMAND_SIZE, stdin);

        if (strcmp(command, "quit\n") == 0)
            return 0;
        else
            if ((pointer = atoi(command))) {
                strcpy(prev_dir, cur_dir);
                if (strcmp(path_list[pointer - 1], "..") == 0)
                    dirwalk("..");
                else dirwalk(path_list[pointer - 1]);
            }
                
    }
    //-------Free memory---------
    for (int i = 0; i < nfiles; i++) {
        free(name_list[i]);
        free(path_list[i]);
    }
    free(name_list);
    free(path_list);

    return 0;
}