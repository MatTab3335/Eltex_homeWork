#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

int main()
{
    /*Spawn a child to run the program.*/
    pid_t pid = fork();
    static char *argv[] = {"ls"};
    switch (pid) { 
        case -1: exit(EXIT_FAILURE);
        case 0:                     /* child process */
            printf("Childs's pid = %i\n", getpid());
            execv("/bin/ls", argv);
            exit(127);              /* only if execv fails */
            break;
        default:                    /* pid!=0; parent process */
            printf("Parents's pid = %i\n", getpid());
            waitpid(pid,0,0);       /* wait for child to exit */
            break;
    }

    exit(0);
}