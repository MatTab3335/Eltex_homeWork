#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char *argv[])
{
    int pipefd[2];
    id_t cpid, cpid1;
    int saved_stdout = dup(STDOUT_FILENO); 

    static char *ls_args[] = {"ls", NULL};
    static char *grep_args[] = {"grep", "te", NULL};
    

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

        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);	 // replace standard output with write end of pipe          
        execvp("ls", ls_args);
        close(pipefd[1]);

        // dup2(saved_stdout, STDOUT_FILENO);	//restore stdout
        // close(saved_stdout);

            
        exit(EXIT_SUCCESS);    
    } 
    else { 
        cpid1 = fork();
        if (cpid1 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid1 == 0) {

            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);     // replace standard input with read end of pipe
            execvp("grep", grep_args);
            close(pipefd[0]);
            exit(EXIT_SUCCESS);
        } else {
            close(pipefd[1]);       //parent close all
            close(pipefd[0]);
            waitpid(cpid1, 0 , 0);
            exit(EXIT_SUCCESS);
        }           
        close(pipefd[0]);          /*Parent Close unused read amd write end */
        close(pipefd[1]);
        waitpid(cpid, 0 , 0);
        exit(EXIT_SUCCESS);
    }
}

