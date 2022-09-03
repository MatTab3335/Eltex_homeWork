//The program accept 1 or 2 commands with '|' deliniter and calls appr. bin
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#define MAX_CMD 256    //max cmd length

char *removeSpaces(char *);

int main(int argc, char *argv[])
{
    int pipefd[2];
    id_t cpid, cpid1;
    char cmd[80] = {};          //to store input string
    char *cmd_t;                 //to store tokens of cmd
    char cmd_list[2][MAX_CMD] = {};
    char arg1[2][MAX_CMD] = {};     //[0] - for name, [1] - only one arguments for bins
    char arg2[2][MAX_CMD] = {};

    
    
    printf("Enter command: ");
    fgets(cmd, 80, stdin);
    
    //------fill the cmd_list-----
    cmd_t = strtok(cmd, "|");
    while (cmd_t != NULL) {
        static int p = 0;
        cmd_t = removeSpaces(cmd_t);
        strcpy(cmd_list[p++], cmd_t);
        cmd_t = strtok(NULL, "|");
    } 
    for (int i = 0; i < 2; i++)
        printf("cmd[%i] = %s\n", i, cmd_list[i]);   

    //------fill the arg lists------
    cmd_t = strtok(cmd_list[0], " ");
    while (cmd_t != NULL) {
        static int p = 0;
        strcpy(arg1[p++], cmd_t);
        cmd_t = strtok(NULL, " ");
    } 
    cmd_t = strtok(cmd_list[1], " ");
    while (cmd_t != NULL) {
        static int p = 0;
        strcpy(arg2[p++], cmd_t);
        
        cmd_t = strtok(NULL, " ");
    }

    printf("arg1 = %s, %s\n", arg1[0], arg1[1]);
    printf("arg2 = %s, %s\n", arg2[0], arg2[1]);

    //----------START PIPE-----------  
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    //----------CREATE CHILD PROCESS-----------
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {    
        id_t cpid1;
        close(pipefd[0]);               // Close unused read end 
        dup2(pipefd[1], 1);                       
        execlp(arg1[0], arg1[0], NULL, NULL);       
        exit(EXIT_SUCCESS);
    } else {
        cpid1 = fork();
        if (cpid1 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid1 == 0) {
            close(pipefd[1]);       //close write
            dup2(pipefd[0], 0);     //copy stdin to pipe read
            execlp(arg2[0], arg2[0], arg2[1], NULL);
            exit(EXIT_SUCCESS);
        } else {
            close(pipefd[1]);       //parent close all
            close(pipefd[0]);
            waitpid(cpid1, 0 , 0);
            exit(EXIT_SUCCESS);
        }            
        close(pipefd[0]);          //Parent Close unused read amd write end 
        close(pipefd[1]);
        waitpid(cpid, 0 , 0);
        exit(EXIT_SUCCESS);
    }

}

char *removeSpaces(char *str) //remove lead and back spaces
{
    char *end;
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0)  // All spaces?
        return str;
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';
    return str;
}