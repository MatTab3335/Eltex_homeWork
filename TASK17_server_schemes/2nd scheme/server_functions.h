#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <signal.h>
#include <semaphore.h>

#define MY_SOCK_PATH "/tmp/tcp_server"
#define LISTEN_BACKLOG 5

#define INIT_THR_NUM 100        //inittial size of thread list


#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

extern int n_of_clients;
//thread variablses
extern pthread_t *thread_list;
//variable to permit sending info msg to client
extern int perm_send;
//global array to mark what thread/server is free (1 - busy, 0 - free)
extern char *serv_stat;


struct args {
    int cfd;
    int id;
};


pthread_t *allocate(pthread_t *list, int init_value);        //alloc list of threads
pthread_t *reallocate(pthread_t *list, int size);      //change size
void SignalHandler(int signal);
void *thr_func(void *input);
int create_server(int domain, int type, char *path);
void handle_error_info(char *msg, char *info);

#endif
