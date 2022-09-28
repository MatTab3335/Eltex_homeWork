//unix clients - scheme #2
#include "client_functions.h"

int main(int argc, char *argv[])
{
    pthread_t thr_list[THR_N];
    int serv_idx[THR_N] = {};
    
    // register signals Ctrl+c
    signal(SIGINT, SignalHandler);

    for (int i = 1; i <= THR_N; i++) {
		serv_idx[i] = i;
        pthread_create(&thr_list[i], NULL, thread_func, (void *) &serv_idx[i]);
        getchar();
        // usleep(70000);
    }
}
