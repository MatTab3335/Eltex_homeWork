//functions for 1st scheme udp
#ifndef _SERVER_FUNCTIONS_H_
#define _SERVER_FUNCTIONS_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <signal.h>

#define MY_SOCK_PATH "/tmp/tcp_server"
#define LISTEN_BACKLOG 5

#define INIT_THR_NUM 10        //inittial size of thread list

// client defs
#define MAIN_SERVER_PATH "/tmp/tcp_server"
#define THR_N 10


#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

extern int n_of_clients;
extern int n_of_threads;
//thread variablses
extern pthread_t *thread_list;
//variable to permit sending info msg to client
extern int perm_send;

pthread_t *allocate(pthread_t *list, int init_value);        //alloc list of threads
pthread_t *reallocate(pthread_t *list, int size);      //change size
void SignalHandler(int signal);
void *thr_func(void *input);
int create_server(int domain, int type, int server_port);
int connect_server(int domain, int type, char *path);
void handle_error_info(char *msg, char *info);

// for client
void *thr_func_client(void *input);
void handle_error_client(char *msg, int id);
void SignalHandlerClient(int signal);

#endif
