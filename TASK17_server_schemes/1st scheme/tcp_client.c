//tcp clients - scheme #1
#include "server_functions.h"

#define MAIN_SERVER_PATH "/tmp/tcp_server"
#define THR_N 1000

int fd;

int main(int argc, char *argv[])
{
    pthread_t thr_list[THR_N];
    int thr_idx[THR_N];
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);

    for (int i = 0; i < THR_N; i++) {
        thr_idx[i] = i;
        pthread_create(&thr_list[i], NULL, thread_func, (void *) &thr_idx[i]);
        usleep(70000);
    }
}




