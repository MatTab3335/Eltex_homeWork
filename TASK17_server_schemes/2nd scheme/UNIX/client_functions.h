// functions for 2nd scheme unix
#ifndef _CLIENT_FUNCTIONS_H_
#define _CLIENT_FUNCTIONS_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define MAIN_SERVER_PATH "/tmp/tcp_server"
#define LISTEN_BACKLOG 5
#define THR_N 100


void SignalHandler(int signal);
void *thread_func(void *input);
void handle_error(char *msg, int id);
void handle_error_info(char *msg, char *info);
int connect_to_serv(int domain, int type, char *path);

#endif

