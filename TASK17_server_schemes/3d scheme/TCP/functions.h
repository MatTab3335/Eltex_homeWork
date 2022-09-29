// functions for 3d scheme tcp
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <mqueue.h>

// server defs
#define MAIN_SERVER_PORT 3000
#define LISTEN_BACKLOG 5
#define INIT_THR_NUM 10        //inittial size of thread list

#define SERVER_QUEUE_NAME   "/3d_scheme_queue"
#define THREADS_QUEUE_NAME   "/3d_scheme_queue_thr"
#define QUEUE_PERMISSIONS 0660      //-rw
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

// client defs
#define THR_N 100


#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

extern int n_of_clients;
extern int n_of_threads;
//thread variablses
extern pthread_t *thread_list;
//global array to mark what thread/server is free (1 - busy, 0 - free)
extern char *serv_stat;

pthread_t *allocate(pthread_t *list, int init_value);        //alloc list of threads
pthread_t *reallocate(pthread_t *list, int size);      //change size
void SignalHandler(int signal);
void *thr_func(void *input);
int create_server(int domain, int type, int server_port);
int connect_server(int domain, int type, int server_port);
void handle_error_char(char *msg, char *info);
void handle_error_int(char *msg, int id);

// for client
void *thr_func_client(void *input);
void SignalHandlerClient(int signal);

#endif
