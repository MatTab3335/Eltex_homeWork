#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    id_t cpid;
    char buf;
    char cmd[80] = {};
    char cmd1[20] = {};
    char cmd2[20] = {};
    
    printf("Enter command: ");
    fgets(cmd, 80, stdin);
    for (int i = 0; i < strlen(cmd); i ++) {
        if (cmd[i] == '|') {
            strncpy(cmd1, cmd, i - 2);
            for (int i1 = i + 2; i1 < strlen(cmd); i1 ++)
                cmd2[i1 - i - 2] = cmd[i1];
            break;
        }
    }
    printf("cmd = %s\n", cmd);
    printf("cmd1 = %s\n", cmd1);
    printf("cmd2 = %s\n", cmd2);
      
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {    
        id_t cpid1;
        
        close(pipefd[0]);               /* Close unused read end */
        if (strlen(cmd2))          
            dup2(1, pipefd[1]);         //copy stdout to pipe write
        execv("/bin", cmd1);
        close(pipefd[1]);

        if (strlen(cmd2)) {             //if there is second command
            cpid1 = fork();
            if (cpid1 == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (cpid1 == 0) {
                close(pipefd[1]);       //close write
                dup2(0, pipefd[0]);     //copy stdin to pipe read
                execv("/bin", cmd2);
                close(pipefd[0]);
                close(pipefd[1]);
                exit(EXIT_SUCCESS);
            } else {
                close(pipefd[1]);       //parent close all
                close(pipefd[0]);
                waitpid(cpid1, 0 , 0);
                exit(EXIT_SUCCESS);
            }
        }
        exit(EXIT_SUCCESS);
    } else {            
        close(pipefd[0]);          /*Parent Close unused read amd write end */
        close(pipefd[1]);
        waitpid(cpid, 0 , 0);
        exit(EXIT_SUCCESS);
    }
}
