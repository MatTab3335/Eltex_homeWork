#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>

#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_BUF_SIZE 10
#define MAX_MSG_SIZE 256

#define PROJECT_ID 'A'

#define SEM_BUFFER_COUNT_NAME   "/tmp/sem-buffer-count"
#define SEM_MSGS_COUNT_NAME     "/tmp/sem-msgs-count"
#define SHARED_MEM_KEY         "/tmp/systemv-shared-mem"

struct sh_mem {
    char buf [MAX_BUF_SIZE][MAX_MSG_SIZE];
    char names [MAX_BUF_SIZE][MAX_MSG_SIZE];
    int name_id;
    int buf_idx;
    int buf_print_idx;
};