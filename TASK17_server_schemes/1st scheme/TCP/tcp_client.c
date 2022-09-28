//tcp clients - scheme #1
#include "functions.h"


int main(int argc, char *argv[])
{
    pthread_t thr_list[THR_N];
    int thr_idx[THR_N];
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);

    for (int i = 0; i < THR_N; i++) {
        thr_idx[i] = i;
        pthread_create(&thr_list[i], NULL, thr_func_client, (void *) &thr_idx[i]);
        usleep(70000);
    }
}




