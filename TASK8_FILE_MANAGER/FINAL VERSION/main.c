#include "functions.h"

int main()
{
	//-------Vars for manager-------
	char command[COMMAND_SIZE];           //user command
    int pointer = 0;
    allocate();
    //-------Init curses----------
    windows_init();
	int key;
	unsigned int highlight = 0;

	//-------Go walk through directories--------
    dirwalk("/"); //*********ТОЛЬКО ОТКРВАЕТ И ЗАПИСЫВАЕТ В МАССИВ------ДОПИСАТЬ ФУНКЦИЮ ПРИНТ
    while(1) {
        printf("Enter the № of file or 'quit' to exit\n");
        fgets(command, COMMAND_SIZE, stdin);

        if (strcmp(command, "quit\n") == 0)
            return 0;
        else
            if ((pointer = atoi(command))) {
                if (strcmp(path_list[pointer - 1], "..") == 0)
                    dirwalk("..");
                else dirwalk(path_list[pointer - 1]);
            }
                
    }
    free_mem();
    windows_del();
}